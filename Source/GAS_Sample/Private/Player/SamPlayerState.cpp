// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

ASamPlayerState::ASamPlayerState()
{
	NetUpdateFrequency = 100.f;
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->SetTeam(ETeam::Player);
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void ASamPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASamPlayerState, Level);
	DOREPLIFETIME(ASamPlayerState, TotalExp);
}

UAbilitySystemComponent* ASamPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASamPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
		PlayerStateList.Add(this);
}

void ASamPlayerState::Destroyed()
{
	Super::Destroyed();
	if(HasAuthority())
		PlayerStateList.Remove(this);
}

UAttributeSet* ASamPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

TArray<TObjectPtr<ACharacter>> ASamPlayerState::GetAllPlayerCharacters()
{
	TArray<TObjectPtr<ACharacter>> Characters;
	
	for (auto PS : PlayerStateList)
	{
		if(APlayerController* PC = PS->GetPlayerController())
		{
			if(ACharacter* Character = PC->GetCharacter())
			{
				Characters.Add(Character);
			}
		}
	}
	return Characters;
}

TArray<FVector> ASamPlayerState::GetAllPlayerCharacterLocations()
{
	TArray<TObjectPtr<ACharacter>> Characters = ASamPlayerState::GetAllPlayerCharacters();
	TArray<FVector> Locations;
	for (auto Character : Characters)
	{
		Locations.Add(Character->GetActorLocation());
	}
	return Locations;
}

void ASamPlayerState::AddToExp(int32 AddedExp)
{
	TotalExp += AddedExp;  
	ExpChangedDelegate.Broadcast(TotalExp);  
}

  
void ASamPlayerState::AddToLevel(int32 AddedLevels)  
{  
	Level+=AddedLevels;  
	LevelChangedDelegate.Broadcast(Level);  
}

int32 ASamPlayerState::FindLevelForExp(int32 ExpValue)
{
	check(LevelUpInfo)
	return LevelUpInfo->FindLevelFromTotalExp(ExpValue);
}

void ASamPlayerState::OnRep_Level(const int32& OldLevel) const
{  
	LevelChangedDelegate.Broadcast(Level);  
}  
  
void ASamPlayerState::OnRep_TotalExp(const int32& OldTotalExp) const
{  
	ExpChangedDelegate.Broadcast(TotalExp);  
}
