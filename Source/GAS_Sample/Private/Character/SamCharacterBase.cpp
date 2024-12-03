// Copyright ZH_Dev

#include "Character/SamCharacterBase.h"

#include "SamGameplayTags.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
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
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->TryActivateAbilitiesByDynamicTag(SamTags::AbilityTags::AbilityTag_ActivateOnDeath);
	SetLifeSpan(1.f);
	GetMovementComponent()->StopMovementImmediately();
	SetActorEnableCollision(false);
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

bool ASamCharacterBase::GetIsActive() const
{
	return bIsActive;
}

FCharacterClassDefaultInfo ASamCharacterBase::GetCharacterClassDefaultInfo()
{
	return USamAbilitySystemLibrary::GetDefaultInfoForCharacterClass(this, CharacterClassTag);
}

void ASamCharacterBase::Auth_ActivateCharacter()
{
	if(!HasAuthority())return;
	bIsActive = true;
	GetMovementComponent()->Activate();
	SetActorHiddenInGame(false);
}

void ASamCharacterBase::Auth_DeactivateCharacter()
{
	if(!HasAuthority())return;
	bIsActive = false;
	GetMovementComponent()->Deactivate();
	SetActorHiddenInGame(true);
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
	USamAttributeSet* SamAttributeSet = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAttributeSet->TagsToAttributes[SamTags::AttributeTags::Attribute_Primary_MovementSpeed]()).AddUObject(this, &ASamCharacterBase::OnMovementSpeedAttributeChanged);
	SetMovementSpeed(SamAttributeSet->TagsToAttributes[SamTags::AttributeTags::Attribute_Primary_MovementSpeed]().GetNumericValue(SamAttributeSet));
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
	GetCharacterMovement()->MaxAcceleration = NewSpeed;
}

void ASamCharacterBase::InitDefaultAttributes()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClassTag, AbilitySystemComponent);
}

void ASamCharacterBase::InitDefaultAbilities()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAbilities(this, CharacterClassTag, AbilitySystemComponent);
}

void ASamCharacterBase::MultiCastHandleDeath_Implementation()
{
	OnDeathDelegate.Broadcast(this);
}
