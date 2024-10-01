// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SamInputConfig.generated.h"

struct FGameplayTag;
class UInputAction;

USTRUCT(BlueprintType)
struct FInputActionTagBinding
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FInputActionTagBinding> InputActopTagBindings;

	const UInputAction* FindAbilityActionForTag(const FGameplayTag& InputTag);
	
};
