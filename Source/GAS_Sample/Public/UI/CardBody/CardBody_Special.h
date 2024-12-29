// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/SpecialFunctionCardAsset.h"
#include "CardBody_Special.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UCardBody_Special : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetInfo(const FSpecialExecCardInfo& Info);
};
