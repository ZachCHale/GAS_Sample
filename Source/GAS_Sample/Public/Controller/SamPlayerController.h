// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "SamPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamPlayerController : public APlayerController
{
	GENERATED_BODY()

	ASamPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void Move(const FInputActionValue& InputActionValue);

	UFUNCTION(BlueprintCallable)
	const FVector& GetCursorWorldPositon() const;

	UFUNCTION(BlueprintCallable)
	const FRotator& GetPawnToCursorRotation() const;

	UFUNCTION(BlueprintCallable)
	const float& GetCursorRange() const;
	

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> SamContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> MoveAction;

	virtual void SetupInputComponent() override;

	void UpdateCursorInformation();
	void RotateControllerToFaceCursorWorldPosition();
	void AdjustCameraDistanceAhead();
	
	FRotator FacingCursorRotation;
	FVector CursorWorldPosition;
	float CursorRange;

	FRotator InitialControllerRotation;
};
