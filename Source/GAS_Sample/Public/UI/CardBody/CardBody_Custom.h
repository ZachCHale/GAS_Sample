// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/CustomCardDatabase.h"
#include "CardBody_Custom.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UCardBody_Custom : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetInfo(const UCustomCardInfo* Info);
};
