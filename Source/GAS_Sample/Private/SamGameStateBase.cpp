// Copyright ZH_Dev


#include "SamGameStateBase.h"

#include "SamLogChannels.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Character/SamCharacterPlayer.h"
#include "Controller/SamPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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
	UE_LOG(SamLog, Log, TEXT("SharedExp %d"), SharedPlayerExp);
}

void ASamGameStateBase::AddToLevel(int32 AddedLevels)
{
	if(!HasAuthority()) return;
	SharedPlayerLevel+=AddedLevels;
	//Broadcast Events to clients
	Multicast_StartLevelUpEvent(SharedPlayerLevel);

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

void ASamGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	if (!PlayerState->IsInactive())
	{
		if(!PlayerArray.Contains(PlayerState))
		{
			PlayerArray.AddUnique(PlayerState);
			LevelUpSelectionState.Add(FPlayerLevelUpSelectionState(PlayerState));
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
			return;
		}
	}
}

void ASamGameStateBase::Auth_ReadyUpPlayerForLevelUpSelection(APlayerState* PlayerState)
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
		UE_LOG(SamLog, Log, TEXT("Should unpause now."));
		for(int i = 0; i < LevelUpSelectionState.Num(); i++)
		{
			LevelUpSelectionState[i].bIsReady = false;
		}
		Multicast_EndLevelUpEvent(SharedPlayerLevel);
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	
}

void ASamGameStateBase::Multicast_StartLevelUpEvent_Implementation(int32 NewLevel)
{
	LevelChangedDelegate.Broadcast(NewLevel);
	//Todo: Generate choices for every player before the UI tries to show the choices.
	BeginLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::OnRep_SharedPlayerLevel(int32 OldLevel) const
{  
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp(int32 OldTotalExp) const
{  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}


