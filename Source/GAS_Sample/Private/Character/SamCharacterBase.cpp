// Copyright ZH_Dev

#include "Character/SamCharacterBase.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"

ASamCharacterBase::ASamCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASamCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* ASamCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
