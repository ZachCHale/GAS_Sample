// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SamGameplayAbility.h"
#include "ProjectileAbility.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UProjectileAbility : public USamGameplayAbility
{
	GENERATED_BODY()

public:

	
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly)
	float DamageValue;

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FTransform& ProjectileSpawnTransform, const FGameplayAbilityActivationInfo& ActivationInfo);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetInstigatorTransform();

};
