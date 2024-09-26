// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "GameFramework/Character.h"

//TArray<TObjectPtr<ASamPlayerState>> ASamPlayerState::PlayerStateList;

ASamPlayerState::ASamPlayerState()
{
	NetUpdateFrequency = 100.f;
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ASamPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASamPlayerState::BeginPlay()
{
	Super::BeginPlay();
	PlayerStateList.Add(this);
}

void ASamPlayerState::Destroyed()
{
	Super::Destroyed();
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
