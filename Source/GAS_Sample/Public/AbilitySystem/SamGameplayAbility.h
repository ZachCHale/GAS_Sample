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
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
protected:
	UPROPERTY(EditDefaultsOnly)
	float CooldownDuration;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CooldownTags;
private:
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
};
