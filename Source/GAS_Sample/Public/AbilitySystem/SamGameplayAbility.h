// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SamGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StartupTag;
	
};
