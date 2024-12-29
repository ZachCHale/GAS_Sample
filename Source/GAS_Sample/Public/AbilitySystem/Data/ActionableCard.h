// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "ActionableCard.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_SAMPLE_API UActionableCard : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void ExecuteCardAction(APlayerState* TargetPlayerState, FGameplayTag ActionCardTag);
	
};
