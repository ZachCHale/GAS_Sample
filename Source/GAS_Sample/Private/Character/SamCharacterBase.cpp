// Copyright ZH_Dev

#include "Character/SamCharacterBase.h"

#include "SamGameplayTags.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

ASamCharacterBase::ASamCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASamCharacterBase::Die()
{
	if(!HasAuthority()) return;
	if(bIsDead) return;
	bIsDead = true;
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->TryActivateAbilitiesByDynamicTag(FSamGameplayTags::Get().AbilityTag_ActivateOnDeath);
	SetLifeSpan(1.f);
	GetMovementComponent()->StopMovementImmediately();
	MultiCastHandleDeath();
}

ETeam ASamCharacterBase::GetTeam()
{
	return CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->GetTeam();
}

bool ASamCharacterBase::GetIsDead() const
{
	return bIsDead;
}

void ASamCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASamCharacterBase::InitAbilityActorInfo()
{
	
}

void ASamCharacterBase::BindToAttributeChanges()
{
	const FSamGameplayTags& GameplayTags = FSamGameplayTags::Get();
	USamAttributeSet* SamAttributeSet = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAttributeSet->TagsToAttributes[GameplayTags.Attribute_Primary_MovementSpeed]()).AddUObject(this, &ASamCharacterBase::OnMovementSpeedAttributeChanged);
	SetMovementSpeed(SamAttributeSet->TagsToAttributes[GameplayTags.Attribute_Primary_MovementSpeed]().GetNumericValue(SamAttributeSet));
}

UAbilitySystemComponent* ASamCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASamCharacterBase::OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	SetMovementSpeed(Data.NewValue);
}

void ASamCharacterBase::SetMovementSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void ASamCharacterBase::InitDefaultAttributes()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, AbilitySystemComponent);
}

void ASamCharacterBase::InitDefaultAbilities()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAbilities(this, CharacterClass, AbilitySystemComponent);
}

void ASamCharacterBase::MultiCastHandleDeath_Implementation()
{
	OnDeathDelegate.Broadcast();
}
