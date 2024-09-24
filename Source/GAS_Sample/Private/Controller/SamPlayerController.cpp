// Copyright ZH_Dev


#include "Controller/SamPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SamLogChannels.h"

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
	UE_LOG(SamLog, Log, TEXT("Using Sam Player Controller"));
}
