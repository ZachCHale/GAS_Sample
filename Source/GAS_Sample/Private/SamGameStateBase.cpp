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

void ASamGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(SamLog, Log, TEXT("State init"))
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(PC == nullptr) return;
	ASamCharacterPlayer* Character = Cast<ASamCharacterPlayer>(PC->GetCharacter());
	if(Character != nullptr)
	{
		if(Character->InitHUD())
		{
			if(HasAuthority())
				UE_LOG(SamLog, Log, TEXT("A"))

			UE_LOG(SamLog, Log, TEXT("Init HUD from Game State suceeded."))
		}else
		{
			if(HasAuthority())
				UE_LOG(SamLog, Log, TEXT("A"))
			UE_LOG(SamLog, Log, TEXT("Init HUD from Game State failed."))
		}
	}
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
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);
	UE_LOG(SamLog, Log, TEXT("SharedLevel %d"), SharedPlayerLevel);

	if(SharedPlayerLevel != 0)
	{
		PauseAndOpenLevelUpScreen();
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

void ASamGameStateBase::OnRep_SharedPlayerLevel(int32 OldLevel) const
{  
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp(int32 OldTotalExp) const
{  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);  
}

void ASamGameStateBase::PauseAndOpenLevelUpScreen()
{
	if(!HasAuthority())return;
	
	GetWorld()->GetFirstPlayerController()->SetPause(true);
}
