// Copyright ZH_Dev


#include "Character/SamCharacterPlayer.h"

#include "SamGameplayTags.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/SamPlayerState.h"
#include "UI/SamWidgetController.h"
#include "UI/HUD/SamHUD.h"


ASamCharacterPlayer::ASamCharacterPlayer()
{
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.f;
	SpringArmComponent->TargetArmLength = 3000.f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(-70.f,0.f,0.f));
}

void ASamCharacterPlayer::InitAbilityActorInfo()
{
	APlayerState* PS = GetPlayerState();
	//If the Player State doesn't exist, it is likely the player is switching to the spectator pawn, so don't bother.
	if(PS == nullptr)return;
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
	AbilitySystemComponent = SamPS->GetAbilitySystemComponent();
	AttributeSet = SamPS->GetAttributeSet();
	AbilitySystemComponent->InitAbilityActorInfo(SamPS, this);
	BindToAttributeChanges();
	Auth_InitDefaultAttributes();
	Auth_InitDefaultAbilities();
	SamPS->InitWithPlayerCharacter(this);
	
	APlayerController* PC = SamPS->GetPlayerController();
	if(PC)
	{
		if(ASamHUD* SamHUD = PC->GetHUD<ASamHUD>())
		{
			const FWidgetControllerParams WidgetControllerParams(PC, SamPS, AbilitySystemComponent, AttributeSet);
			SamHUD->InitOverlay(WidgetControllerParams);
		}
	}
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

void ASamCharacterPlayer::SetCameraDistanceAhead(float Distance)
{
	SpringArmComponent->SetRelativeLocation(FVector(Distance ,0.f,0.f));
}

int32 ASamCharacterPlayer::GetLevel()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->GetLevel();
}

int32 ASamCharacterPlayer::GetTotalExp()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->GetTotalExp();
}

void ASamCharacterPlayer::Auth_AddToExp(int32 AddedExp)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->Auth_AddToExp(AddedExp);
}

void ASamCharacterPlayer::Auth_AddToLevel(int32 AddedLevels)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->Auth_AddToLevel(AddedLevels);
}

int32 ASamCharacterPlayer::FindLevelForExp(int32 ExpValue)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->FindLevelForExp(ExpValue);
}

void ASamCharacterPlayer::Auth_Die()
{
	if(!HasAuthority()) return;
	if(bIsDead) return;
	bIsDead = true;
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->TryActivateAbilitiesByDynamicTag(SamTags::AbilityTags::AbilityTag_ActivateOnDeath);
	GetMovementComponent()->StopMovementImmediately();
	SetActorEnableCollision(false);
	MultiCastHandleDeath();
}

void ASamCharacterPlayer::Auth_Revive()
{
	if(!HasAuthority()) return;
	bIsDead = false;
	SetActorEnableCollision(true);
	MultiCastHandleRevive();
}

void ASamCharacterPlayer::MultiCastHandleRevive_Implementation()
{
	OnReviveDelegate.Broadcast(this);
}
