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
	//TODO: Handle Edge case of multiple level ups, keep a backlogged count of level ups, when we go to unpause, open selection again instead.
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

void ASamGameStateBase::Auth_ReadyUpPlayerForLevelUpSelection(const APlayerState* PlayerState)
{
	check(HasAuthority());
	bool bAreAllPlayersReady = true;
	for (int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			LevelUpSelectionState[i].bIsReady = true;
		}
		bAreAllPlayersReady &= LevelUpSelectionState[i].bIsReady;
	}
	if(bAreAllPlayersReady)
	{
		for(int i = 0; i < LevelUpSelectionState.Num(); i++)
		{
			LevelUpSelectionState[i].bIsReady = false;
		}
		Multicast_EndLevelUpEvent(SharedPlayerLevel);
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	
}

void ASamGameStateBase::Auth_SendPlayerLevelUpSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	check(HasAuthority());
	if(StateStatus != EGameStateStatus::LevelUpSelection)return;
	bool bAreAllPlayersReady = true;
	int32 ReadyCount = 0;
	for (int32 i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			LevelUpSelectionState[i].bIsReady = true;
			LevelUpSelectionState[i].CurrentlySelectedChoice = UpgradeTag;
		}
		if(LevelUpSelectionState[i].bIsReady)
			ReadyCount++;
		bAreAllPlayersReady &= LevelUpSelectionState[i].bIsReady;
	}
	int32 PrevReadyCount = PlayerReadyCount;
	PlayerReadyCount = ReadyCount;
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
	if(bAreAllPlayersReady)
	{
		for(int32 i = 0; i < LevelUpSelectionState.Num(); i++)
		{
			ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(LevelUpSelectionState[i].PlayerState);
			USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
			SamASC->Auth_IncrementUpgradeEffect(LevelUpSelectionState[i].CurrentlySelectedChoice);
			LevelUpSelectionState[i].ResetSelectionState();
		}
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
}

bool ASamGameStateBase::IsValidUpgradeSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	for (int32 i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			//Invalid Choice
			if(!LevelUpSelectionState[i].UpgradeChoiceTags.Contains(UpgradeTag))
				return false;
			break;
		}
	}
	return true;
}

void ASamGameStateBase::Auth_ClearPlayerLevelUpSelection(APlayerState* PlayerState)
{
	check(HasAuthority());
	int32 ReadyCount = 0;
	for (int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			LevelUpSelectionState[i].bIsReady = false;
			LevelUpSelectionState[i].CurrentlySelectedChoice = FGameplayTag();
		}
		else if(LevelUpSelectionState[i].bIsReady)
		{
			ReadyCount++;
		}
	}
	int32 PrevCount = PlayerReadyCount;
	PlayerReadyCount = ReadyCount;
	//OnRep_PlayerReadyCount(PrevCount);
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
			for (auto Tag : PlayerSelectionState.UpgradeChoiceTags)
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




