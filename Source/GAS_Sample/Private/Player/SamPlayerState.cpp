// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"

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

UAttributeSet* ASamPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
