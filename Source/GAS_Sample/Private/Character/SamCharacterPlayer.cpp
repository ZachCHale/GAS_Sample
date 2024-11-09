// Copyright ZH_Dev


#include "Character/SamCharacterPlayer.h"

#include "SamGameplayTags.h"
#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
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
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	AbilitySystemComponent = SamPS->GetAbilitySystemComponent();
	AttributeSet = SamPS->GetAttributeSet();
	AbilitySystemComponent->InitAbilityActorInfo(SamPS, this);
	BindToAttributeChanges();
	InitDefaultAttributes();
	InitDefaultAbilities();

	if(InitHUD())
	{
		if(HasAuthority())
			UE_LOG(SamLog, Log, TEXT("A"))
		UE_LOG(SamLog, Log, TEXT("Init HUD from Character suceeded."))
	}else
	{
		if(HasAuthority())
			UE_LOG(SamLog, Log, TEXT("A"))
		UE_LOG(SamLog, Log, TEXT("Init HUD from Character failed."))
	}
	/*APlayerController* PC = SamPS->GetPlayerController();
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	if(PC)
	{
		if(ASamHUD* SamHUD = PC->GetHUD<ASamHUD>())
		{
			const FWidgetControllerParams WidgetControllerParams(PC, SamPS, AbilitySystemComponent, AttributeSet);
			SamHUD->InitOverlay(WidgetControllerParams);
		}
	}*/
}

void ASamCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//UE_LOG(SamLog, Log, TEXT("OnRep_PlayerState"));
	
	InitAbilityActorInfo();
}

void ASamCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//UE_LOG(SamLog, Log, TEXT("Possessed"));
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

void ASamCharacterPlayer::AddToExp(int32 AddedExp)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->AddToExp(AddedExp);
}

void ASamCharacterPlayer::AddToLevel(int32 AddedLevels)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->AddToLevel(AddedLevels);
}

int32 ASamCharacterPlayer::FindLevelForExp(int32 ExpValue)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(GetPlayerState());
	return SamPS->FindLevelForExp(ExpValue);
}

bool ASamCharacterPlayer::InitHUD()
{
	//Player State and Game State can be replicated in to client in any order, into hud gets called from both.
	ASamPlayerState* SamPS = Cast<ASamPlayerState>(GetPlayerState());

	if (SamPS == nullptr)
	{
		UE_LOG(SamLog, Warning, TEXT("Init HUD postponed, Playerstate not ready"));
		return false;
	}

	APlayerController* PC = SamPS->GetPlayerController();
	
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);

	if(SamGS == nullptr)
	{
		UE_LOG(SamLog, Warning, TEXT("Init HUD postponed, Gamestate not ready."));
		return false;
	}
	
	if(PC)
	{
		if(ASamHUD* SamHUD = PC->GetHUD<ASamHUD>())
		{
			const FWidgetControllerParams WidgetControllerParams(PC, SamPS, AbilitySystemComponent, AttributeSet);
			SamHUD->InitOverlay(WidgetControllerParams);
			return true;
		}
	}
	return false;
}
