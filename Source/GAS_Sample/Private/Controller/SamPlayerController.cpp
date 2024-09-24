// Copyright ZH_Dev


#include "Controller/SamPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SamLogChannels.h"
#include "EnhancedInputComponent.h"

ASamPlayerController::ASamPlayerController()
{
	bReplicates = true;
}

void ASamPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(SamContext);
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(SubSystem);
	SubSystem->AddMappingContext(SamContext, 0);
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior((EMouseLockMode::DoNotLock));
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	check(MoveAction);
}

void ASamPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASamPlayerController::Move);
}

void ASamPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);  
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if(APawn* ControlledPawn = GetPawn<APawn>())  
	{  
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);  
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);  
	}
}
