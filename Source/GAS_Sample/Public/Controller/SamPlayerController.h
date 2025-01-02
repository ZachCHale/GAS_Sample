// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "SamPlayerController.generated.h"

class ASamCharacterBase;
class USamInputConfig;
class USamAbilitySystemComponent;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASamPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	void Move(const FInputActionValue& InputActionValue);

	UFUNCTION(BlueprintCallable)
	const FVector& GetCursorWorldPosition() const;

	UFUNCTION(BlueprintCallable)
	const FRotator& GetPawnToCursorRotation() const;

	UFUNCTION(BlueprintCallable)
	const float& GetCursorRange() const;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Sever_SendLevelUpSelection(FGameplayTag UpgradeTag);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Sever_SendReadyUpLobby();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Sever_SendPauseRequest();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Sever_SendUnpauseRequest();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Sever_ClearLevelUpSelection();
	
	void Auth_StartSpectating();
	
	void Auth_StopSpectating();

	

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_SpectateNext();
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_SpectatePrev();

protected:
	virtual void SetupInputComponent() override;


private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> SamContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> MoveAction;

	bool bIsSpectating;
	//Index of live character being viewed when in spectator mode
	int32 ViewTargetIndex;

	void UpdateCursorInformation();
	void RotateControllerToFaceCursorWorldPosition();
	void AdjustCameraDistanceAhead();
	
	FRotator FacingCursorRotation;
	FVector CursorWorldPosition;
	float CursorRange;

	FRotator InitialControllerRotation;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USamInputConfig> InputConfig;

	void InputActionPressed(FGameplayTag BoundTag);

	void InputActionReleased(FGameplayTag BoundTag);

	void InputActionHeld(FGameplayTag BoundTag);

	UPROPERTY()
	TObjectPtr<USamAbilitySystemComponent> SamAbilitySystemComponent;

	USamAbilitySystemComponent* GetASC();

	UFUNCTION()
	void HandleViewTargetDeath(ASamCharacterBase* CharacterInstance);

	UFUNCTION()
	void HandleControlledCharacterDeath(ASamCharacterBase* CharacterInstance);

	UFUNCTION()
	void HandleControlledCharacterRevive(ASamCharacterBase* CharacterInstance);

	

};
