// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SamGameplayAbility.h"
#include "ItemDropAbility.generated.h"

UCLASS()
class GAS_SAMPLE_API UItemDropAbility : public USamGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	//Use CustomSpawnAmountCalculation() for spawn amount. Defaults to 1 if not overridden.
	UPROPERTY(EditDefaultsOnly)
	bool bUseCustomSpawnAmountCalculation = false;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bUseCustomSpawnAmountCalculation"))
	int32 SpawnAmount = 1;

	//Adds impulse to items in a random direction if the root component is a UPrimitive.
	UPROPERTY(EditDefaultsOnly)
	float ThrowItemStrength = 0;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ItemClass;

	UFUNCTION(BlueprintNativeEvent)
	void InitItemExtraData(AActor* ItemInstance);

	UFUNCTION(BlueprintNativeEvent)
	void InitItemReplication(AActor* ItemInstance);
	
	UFUNCTION(BlueprintNativeEvent)
	FTransform InitItemTransform();
	
	void AddImpulseToItem(const AActor* ItemInstance) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 CustomSpawnAmountCalculation();
};
