// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/UpgradeCardDatabase.h"
#include "CardBody_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UCardBody_Upgrade : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetInfo(const UUpgradeCardInfo* Info);
	
};
