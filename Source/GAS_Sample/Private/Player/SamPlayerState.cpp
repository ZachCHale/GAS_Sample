// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

#include "SamGameStateBase.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
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

int32 ASamPlayerState::GetLevel()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->GetLevel();
}

int32 ASamPlayerState::GetTotalExp()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->GetTotalExp();
}

void ASamPlayerState::AddToExp(int32 AddedExp)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->AddToExp(AddedExp);
}

  
void ASamPlayerState::AddToLevel(int32 AddedLevels)  
{  
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->AddToLevel(AddedLevels);
}

int32 ASamPlayerState::FindLevelForExp(int32 ExpValue)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->FindLevelForExp(ExpValue);
}
