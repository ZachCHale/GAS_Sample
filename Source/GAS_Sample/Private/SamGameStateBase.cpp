// Copyright ZH_Dev


#include "SamGameStateBase.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Character/SamCharacterPlayer.h"
#include "Controller/SamPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/SamPlayerState.h"
#include "UI/HUD/SamHUD.h"

void ASamGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerLevel);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerExp);
}

void ASamGameStateBase::AddToExp(int32 AddedExp)
{
	if(!HasAuthority()) return;
	SharedPlayerExp += AddedExp;  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::AddToLevel(int32 AddedLevels)
{
	if(!HasAuthority()) return;
	if(AddedLevels <= 0) return;
	StateStatus = EGameStateStatus::LevelUpSelection;
	QueuedLevelUps = AddedLevels;
	QueuedLevelUps--;
	SharedPlayerLevel++;
	Auth_GenerateUpgradesForPlayers();
	//Broadcast Events to clients
	Multicast_StartLevelUpEvent(SharedPlayerLevel, SharedPlayerExp, LevelUpSelectionState);
	//Pause Game
	if(SharedPlayerLevel != 0)
	{
		GetWorld()->GetFirstPlayerController()->SetPause(true);
	}
}

int32 ASamGameStateBase::FindLevelForExp(int32 ExpValue)
{
	check(LevelUpInfo)
	return LevelUpInfo->FindLevelFromTotalExp(ExpValue);
}

TArray<ACharacter*> ASamGameStateBase::GetAllPlayerCharacters()
{
	TArray<TObjectPtr<ACharacter>> Characters;

	for (auto PS : PlayerArray)
	{
		if(APlayerController* PC = PS->GetPlayerController())
		{
			if(ACharacter* Character = PC->GetCharacter())
			{
				Characters.AddUnique(Character);
			}
		}
	}
	return Characters;
}

void ASamGameStateBase::Multicast_EndLevelUpEvent_Implementation(int32 NewLevel)
{
	EndLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_UpdateReadyCount_Implementation(int32 NewReady, int32 NewTotal)
{
	PlayerReadyCount = NewReady;
	PlayerReadyCountChangedDelegate.Broadcast(NewReady, NewTotal);
}

void ASamGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	if (!PlayerState->IsInactive())
	{
		if(!PlayerArray.Contains(PlayerState))
		{
			PlayerArray.AddUnique(PlayerState);
			LevelUpSelectionState.Add(FPlayerLevelUpSelectionState(PlayerState));
			PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
		}
	}
}

void ASamGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	for(int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(PlayerState == LevelUpSelectionState[i].PlayerState)
		{
			LevelUpSelectionState.RemoveAt(i);
			PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
			return;
		}
	}
}

void ASamGameStateBase::Auth_SendPlayerLevelUpSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	check(HasAuthority());
	if(StateStatus != EGameStateStatus::LevelUpSelection)return;
	
	if(FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState))
	{
		PlayerSelectionState->bIsReady = true;
		PlayerSelectionState->CurrentlySelectedChoice = UpgradeTag;
	}

	PlayerReadyCount = CountPlayerSelectionsReady();
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
	
	if(PlayerReadyCount == PlayerArray.Num())
	{
		Auth_SubmitAllPlayerUpgradeSelections();
	}
}

bool ASamGameStateBase::IsValidUpgradeSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState);
	if(!PlayerSelectionState || !PlayerSelectionState->UpgradeChoiceTags.Contains(UpgradeTag)) return false;
	return true;
}

void ASamGameStateBase::Auth_ClearPlayerLevelUpSelection(APlayerState* PlayerState)
{
	check(HasAuthority());
	if(FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState))
	{
		PlayerSelectionState->bIsReady = false;
		PlayerSelectionState->CurrentlySelectedChoice = FGameplayTag();
	}
	PlayerReadyCount = CountPlayerSelectionsReady();
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
}

TArray<FUpgradeInfoItem> ASamGameStateBase::GetLocalPlayerSelectionChoices()
{
	TArray<FUpgradeInfoItem> ChoicesInfo;
	for (auto PlayerSelectionState : LevelUpSelectionState)
	{
		//If on client, remote player states won't have a controller
		if(PlayerSelectionState.PlayerState->GetPlayerController() == nullptr)
			continue;
		if(PlayerSelectionState.PlayerState->GetPlayerController()->IsLocalPlayerController())
		{
			for (FGameplayTag Tag : PlayerSelectionState.UpgradeChoiceTags)
			{
				ChoicesInfo.Add(UpgradeInfo->GetUpgradeInfoFromTag(Tag));
			}
			break;
		}
	}
	return ChoicesInfo;
}

void ASamGameStateBase::Multicast_StartLevelUpEvent_Implementation(int32 NewLevel, int32 NewExp, const TArray<FPlayerLevelUpSelectionState>& UpdatedLevelUpSelectionState)
{
	//Replication doesn't seem to work when the game is paused, so we need to manually replicate from the RPC
	SharedPlayerExp = NewExp;
	OnRep_SharedPlayerExp();
	SharedPlayerLevel = NewLevel;
	OnRep_SharedPlayerLevel();
	
	//Server generates the upgrade choices and sends them via this RPC
	//Replicate the relevant data for Upgrade selection before broadcasting UpgradeSelectionDelegate.
	LevelUpSelectionState = UpdatedLevelUpSelectionState;
	
	BeginLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::OnRep_SharedPlayerLevel() const
{
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp() const
{
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::OnRep_PlayerReadyCount() const
{
	PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
}

void ASamGameStateBase::Auth_GenerateUpgradesForPlayers()
{
	if(!HasAuthority()) return;
	for (auto& PlayerLevelUpState : LevelUpSelectionState)
	{
		PlayerLevelUpState.ResetSelectionState();
		PlayerLevelUpState.UpgradeChoiceTags = UpgradeInfo->GetRandomUpgradeTags(3);
	}

}

FPlayerLevelUpSelectionState* ASamGameStateBase::GetPlayerLevelUpSelectionState(const APlayerState* PlayerState)
{
	for (int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			return &LevelUpSelectionState[i];
		}
	}
	return nullptr;
}

int32 ASamGameStateBase::CountPlayerSelectionsReady()
{
	int32 ReadyCount = 0;
	for (auto PlayerLevelUpSelection : LevelUpSelectionState)
	{
		if(PlayerLevelUpSelection.bIsReady) ReadyCount++;
	}
	return ReadyCount;
}

void ASamGameStateBase::Auth_SubmitAllPlayerUpgradeSelections()
{
	if(!HasAuthority()) return;
	//Increase Level / Create new effect for selected upgrades.
	for(int32 i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(LevelUpSelectionState[i].PlayerState);
		USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
		SamASC->Auth_IncrementUpgradeEffect(LevelUpSelectionState[i].CurrentlySelectedChoice);
		LevelUpSelectionState[i].ResetSelectionState();
	}
	
	//Handle Simultaneous Level ups, or resume gameplay
	if(QueuedLevelUps <=0)
	{
		StateStatus = EGameStateStatus::Gameplay;
		Multicast_EndLevelUpEvent(SharedPlayerLevel);
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	else
	{
		AddToLevel(QueuedLevelUps);
	}
}




