// Copyright ZH_Dev


#include "Controller/SamPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Character/SamCharacterPlayer.h"
#include "Input/SamInputComponent.h"
#include "Player/SamPlayerState.h"

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

void ASamPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//Bind to character events
	if(ASamCharacterPlayer* SamChar = Cast<ASamCharacterPlayer>(InPawn))
	{
		SamChar->OnReviveDelegate.AddUniqueDynamic(this, &ThisClass::HandleControlledCharacterRevive);
		SamChar->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleControlledCharacterDeath);
	}
}

void ASamPlayerController::Sever_SendPauseRequest_Implementation()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->Auth_PauseGame();
}

void ASamPlayerController::Sever_SendUnpauseRequest_Implementation()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->Auth_UnpauseGame();
	SamGS->Multicast_GameUnpausedByPlayer();
}

void ASamPlayerController::Auth_StartSpectating()
{
	if(!HasAuthority())return;
	//ASamCharacterPlayer* SamChar = Cast<ASamCharacterPlayer>(GetCharacter());
	//
	bIsSpectating = true;
	ViewTargetIndex = 0;
	TArray<ACharacter*> LiveCharacters = USamAbilitySystemLibrary::GetLivePlayerCharacters(this);
	if(LiveCharacters.Num() <= 0) return;
	ASamCharacterBase* SamCharacter = Cast<ASamCharacterBase>(LiveCharacters[0]);
	SetViewTarget(SamCharacter);
	SamCharacter->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleViewTargetDeath);
}

void ASamPlayerController::Auth_StopSpectating()
{
	if(!HasAuthority())return;
	bIsSpectating = false;
	
	//Unbind from previous
	ASamCharacterBase* PrevTarget = Cast<ASamCharacterBase>(GetViewTarget());
	PrevTarget->OnDeathDelegate.RemoveDynamic(this, &ThisClass::HandleViewTargetDeath);
	
	ASamCharacterPlayer* SamChar = Cast<ASamCharacterPlayer>(GetCharacter());
	SetViewTarget(SamChar);
}


void ASamPlayerController::Server_SpectateNext_Implementation()
{
	if(!bIsSpectating)return;
	TArray<ACharacter*> LiveCharacters = USamAbilitySystemLibrary::GetLivePlayerCharacters(this);
	if(LiveCharacters.Num() <= 0) return;

	//Unbind from previous
	ASamCharacterBase* PrevTarget = Cast<ASamCharacterBase>(LiveCharacters[ViewTargetIndex]);
	PrevTarget->OnDeathDelegate.RemoveDynamic(this, &ThisClass::HandleViewTargetDeath);
	//Change index
	ViewTargetIndex++;
	if(ViewTargetIndex >= LiveCharacters.Num())
		ViewTargetIndex = 0;
	if(ViewTargetIndex < 0)
		ViewTargetIndex = LiveCharacters.Num()-1;
	//Switch view and bind to new target
	ASamCharacterBase* NextTarget = Cast<ASamCharacterBase>(LiveCharacters[ViewTargetIndex]);
	SetViewTarget(NextTarget);
	NextTarget->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleViewTargetDeath);
}


void ASamPlayerController::Server_SpectatePrev_Implementation()
{
	if(!bIsSpectating)return;
	TArray<ACharacter*> LiveCharacters = USamAbilitySystemLibrary::GetLivePlayerCharacters(this);
	if(LiveCharacters.Num() <= 0) return;
	
	//Unbind from previous
	ASamCharacterBase* PrevTarget = Cast<ASamCharacterBase>(LiveCharacters[ViewTargetIndex]);
	PrevTarget->OnDeathDelegate.RemoveDynamic(this, &ThisClass::HandleViewTargetDeath);
	//Change index
	ViewTargetIndex--;
	if(ViewTargetIndex >= LiveCharacters.Num())
		ViewTargetIndex = 0;
	if(ViewTargetIndex < 0)
		ViewTargetIndex = LiveCharacters.Num()-1;
	//Switch view and bind to new target
	ASamCharacterBase* NextTarget = Cast<ASamCharacterBase>(LiveCharacters[ViewTargetIndex]);
	SetViewTarget(NextTarget);
	NextTarget->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleViewTargetDeath);	
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
	APawn* ControlledPawn = GetPawn<APawn>();
	ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn);
	if(SamCharacterPlayer==nullptr || SamCharacterPlayer->GetIsDead())return;
	
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
	if(GetASC() == nullptr) return;

	APawn* ControlledPawn = GetPawn<APawn>();
	ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn);
	if(SamCharacterPlayer==nullptr || SamCharacterPlayer->GetIsDead()) return;
	
}

void ASamPlayerController::InputActionReleased(FGameplayTag BoundTag)
{
	if(GetASC() == nullptr) return;
	APawn* ControlledPawn = GetPawn<APawn>();
	ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn);
	if(SamCharacterPlayer==nullptr || SamCharacterPlayer->GetIsDead())return;
	
	GetASC()->AbilityInputReleased(BoundTag);
}

void ASamPlayerController::InputActionHeld(FGameplayTag BoundTag)
{
	if(GetASC() == nullptr) return;
	
	APawn* ControlledPawn = GetPawn<APawn>();
	ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn);
	if(SamCharacterPlayer==nullptr || SamCharacterPlayer->GetIsDead())return;
	
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

void ASamPlayerController::HandleViewTargetDeath(ASamCharacterBase* CharacterInstance)
{
	Auth_StartSpectating();
}

void ASamPlayerController::HandleControlledCharacterDeath(ASamCharacterBase* CharacterInstance)
{
	Auth_StartSpectating();
}

void ASamPlayerController::HandleControlledCharacterRevive(ASamCharacterBase* CharacterInstance)
{
	Auth_StopSpectating();
}


void ASamPlayerController::Move(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn<APawn>();
	ASamCharacterPlayer* SamCharacterPlayer = Cast<ASamCharacterPlayer>(ControlledPawn);
	if(SamCharacterPlayer==nullptr || SamCharacterPlayer->GetIsDead())return;

	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = InitialControllerRotation;
	const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);  
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	if(ControlledPawn)  
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
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	SamPS->Auth_ReadyUpPlayerLobbyState();
}

bool ASamPlayerController::Sever_SendLevelUpSelection_Validate(FGameplayTag UpgradeTag)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	return SamPS->IsUpgradeSelectionValid(UpgradeTag);
}

void ASamPlayerController::Sever_SendLevelUpSelection_Implementation(FGameplayTag UpgradeTag)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	SamPS->Auth_SubmitUpgradeSelection(UpgradeTag);
}

void ASamPlayerController::Sever_ClearLevelUpSelection_Implementation()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	SamPS->Auth_ClearUpgradeSelection();
}
