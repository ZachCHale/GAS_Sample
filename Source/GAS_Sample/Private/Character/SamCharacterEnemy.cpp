// Copyright ZH_Dev


#include "Character/SamCharacterEnemy.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"

ASamCharacterEnemy::ASamCharacterEnemy()
{
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");
}

void ASamCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
