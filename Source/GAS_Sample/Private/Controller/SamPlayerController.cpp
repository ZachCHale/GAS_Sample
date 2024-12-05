// Copyright ZH_Dev


#include "Controller/SamPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SamGameStateBase.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Character/SamCharacterPlayer.h"
#include "Input/SamInputComponent.h"

ASamPlayerController::ASamPlayerController()
{
	bReplicates = true;
}

void ASamPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(SamContext);
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(SubSystem)
	{
		SubSystem->AddMappingContext(SamContext, 0);
		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Type::Default;
		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior((EMouseLockMode::DoNotLock));
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
	}
	
	InitialControllerRotation = FRotator();
	check(MoveAction);
}

void ASamPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCursorInformation();
	RotateControllerToFaceCursorWorldPosition();
	AdjustCameraDistanceAhead();
}

void ASamPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	USamInputComponent* SamInputComponent = CastChecked<USamInputComponent>(InputComponent);
	SamInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASamPlayerController::Move);
	SamInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::InputActionPressed, &ThisClass::InputActionReleased, &ThisClass::InputActionHeld);
}

void ASamPlayerController::UpdateCursorInformation()
{
	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		if(!HitResult.bBlockingHit) return;
		CursorWorldPosition = HitResult.Location;
		
		FVector PawnPosition = ControlledPawn->GetActorLocation();
		FacingCursorRotation = ((CursorWorldPosition - PawnPosition) * FVector(1,1,0)).Rotation();

		int32 ViewportSizeX;
		int32 ViewportSizeY;
		GetViewportSize(ViewportSizeX, ViewportSizeY);
		float MouseX;
		float MouseY;
		GetMousePosition(MouseX, MouseY);
		float TX = FMathf::Clamp((MouseX-(ViewportSizeX-ViewportSizeY)/2) / static_cast<float>(ViewportSizeY), 0.f, 1.f);
		float TY = FMathf::Clamp(MouseY / static_cast<float>(ViewportSizeY), 0.f,1.f);
		CursorRange = FMathf::Clamp( FMathf::Abs((FVector2f(TX * 2 - 1,TY * 2 - 1).Length())),0.f, 1.f);
	}
}

void ASamPlayerController::RotateControllerToFaceCursorWorldPosition()
{
	ControlRotation = FacingCursorRotation;
}

void ASamPlayerController::AdjustCameraDistanceAhead()
{
	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		if(ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn))
		{
			float CameraRange = FMath::GetMappedRangeValueClamped(FVector2f(.25f,.75f), FVector2f(0.f,1.f), CursorRange);
			SamCharacterPlayer->SetCameraDistanceAhead(CameraRange * 1000);
		}
	}
}

void ASamPlayerController::InputActionPressed(FGameplayTag BoundTag)
{
	
	
}

void ASamPlayerController::InputActionReleased(FGameplayTag BoundTag)
{
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputReleased(BoundTag);
}

void ASamPlayerController::InputActionHeld(FGameplayTag BoundTag)
{
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputHeld(BoundTag);
}

USamAbilitySystemComponent* ASamPlayerController::GetASC()
{
	if(SamAbilitySystemComponent == nullptr)
	{
		SamAbilitySystemComponent = Cast<USamAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return SamAbilitySystemComponent;
}


void ASamPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = InitialControllerRotation;
	const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);  
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	if(APawn* ControlledPawn = GetPawn<APawn>())  
	{  
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);  
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);  
	}
}

const FVector& ASamPlayerController::GetCursorWorldPosition() const
{
	return CursorWorldPosition;
}

const FRotator& ASamPlayerController::GetPawnToCursorRotation() const
{
	return FacingCursorRotation;
}

const float& ASamPlayerController::GetCursorRange() const
{
	return CursorRange;
}

void ASamPlayerController::Sever_SendReadyUpLobby_Implementation()
{
	if(ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this))
	{
		SamGS->Auth_SetPlayerReadyInLobby(PlayerState);
	}
}

bool ASamPlayerController::Sever_SendLevelUpSelection_Validate(FGameplayTag UpgradeTag)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	if(SamGS && SamGS->IsValidUpgradeSelection(PlayerState, UpgradeTag))
	{
		return true;
	}
	return false;
}

void ASamPlayerController::Sever_SendLevelUpSelection_Implementation(FGameplayTag UpgradeTag)
{
	if(ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this))
	{
		SamGS->Auth_SendPlayerLevelUpSelection(PlayerState, UpgradeTag);
	}
}

void ASamPlayerController::Sever_ClearLevelUpSelection_Implementation()
{
	if(ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this))
	{
		SamGS->Auth_ClearPlayerLevelUpSelection(PlayerState);
	}
}
