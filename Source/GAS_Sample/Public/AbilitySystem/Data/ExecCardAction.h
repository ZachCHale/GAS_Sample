// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "ExecCardAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class GAS_SAMPLE_API UExecCardAction : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteAction(APlayerState* TargetPlayer, FGameplayTag ActionTag);
	
};
