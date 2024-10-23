// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SamGameplayTags.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
protected:
private:
};
