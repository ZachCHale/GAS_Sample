// Copyright ZH_Dev


#include "Character/SamCharacterPlayer.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/SamPlayerState.h"

ASamCharacterPlayer::ASamCharacterPlayer()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void ASamCharacterPlayer::InitAbilityActorInfo()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	AbilitySystemComponent = SamPS->GetAbilitySystemComponent();
	AttributeSet = SamPS->GetAttributeSet();
	AbilitySystemComponent->InitAbilityActorInfo(SamPS, this);
}

void ASamCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void ASamCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}
