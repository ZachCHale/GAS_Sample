// Copyright ZH_Dev

#include "Character/SamCharacterBase.h"

#include "SamGameplayTags.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassDatabase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ASamCharacterBase::ASamCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASamCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, MovementSpeed);
}

void ASamCharacterBase::Auth_Die()
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
	Auth_SetMovementSpeed(SamAttributeSet->TagsToAttributes[SamTags::AttributeTags::Attribute_Primary_MovementSpeed]().GetNumericValue(SamAttributeSet));
}

UAbilitySystemComponent* ASamCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASamCharacterBase::OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	Auth_SetMovementSpeed(Data.NewValue);
}

void ASamCharacterBase::Auth_SetMovementSpeed(float NewSpeed)
{
	if(!HasAuthority())	return;
	//This variable will get replicated to client, where it will update the client side movment component as well
	MovementSpeed = NewSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MaxAcceleration = MovementSpeed;
}

void ASamCharacterBase::Auth_InitDefaultAttributes()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClassTag, AbilitySystemComponent);
}

void ASamCharacterBase::Auth_InitDefaultAbilities()
{
	if(!HasAuthority()) return;
		USamAbilitySystemLibrary::InitializeDefaultAbilities(this, CharacterClassTag, AbilitySystemComponent);
}

void ASamCharacterBase::OnRep_MovementSpeed() const
{
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MaxAcceleration = MovementSpeed;
}

void ASamCharacterBase::MultiCastHandleDeath_Implementation()
{
	OnDeathDelegate.Broadcast(this);
}
