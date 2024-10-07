// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SamGameplayAbility.h"
#include "ItemDropAbility.generated.h"

USTRUCT(BlueprintType)
struct FItemDrop
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> DropObjectClass;
	UPROPERTY(EditDefaultsOnly)
	int32 MinimumAmount;
	UPROPERTY(EditDefaultsOnly)
	int32 MaximumAmount;
};

USTRUCT(BlueprintType)
struct FItemDropGroup
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TArray<FItemDrop> Items;
	UPROPERTY(EditDefaultsOnly)
	int32 DropWeight;
};

USTRUCT(BlueprintType)
struct FItemDropRoll
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TArray<FItemDropGroup> PossibleItems;
};

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UItemDropAbility : public USamGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FItemDropRoll> DropTables;
private:
	void DropItems(const FItemDropGroup& ItemGroup);

};
