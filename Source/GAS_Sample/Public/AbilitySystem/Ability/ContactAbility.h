// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SamGameplayAbility.h"
#include "ContactAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UContactAbility : public USamGameplayAbility
{
	GENERATED_BODY()
public:
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UFUNCTION(BlueprintCallable)
	void Auth_ApplyToActorsInContactRange(const FGameplayAbilityActivationInfo& ActivationInfo);
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, float> DamageTypeValues;
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect>  GameplayEffectClass;
	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugSphere;
	UPROPERTY(EditDefaultsOnly)
	float ContactRadius;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
};
