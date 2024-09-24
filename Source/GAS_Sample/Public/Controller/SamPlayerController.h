// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
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

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> SamContext;
};
