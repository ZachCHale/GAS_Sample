// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Actor/Interface/TeamInterface.h"
#include "SamAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamAbilitySystemComponent : public UAbilitySystemComponent, public ITeamInterface
{
	GENERATED_BODY()

public:
	void AbilityInputHeld(FGameplayTag GameplayTag);
	void AbilityInputReleased(FGameplayTag GameplayTag);

	virtual ETeam GetTeam() override;
	void SetTeam(ETeam InTeam);
	
	void TryActivateAbilitiesByDynamicTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentUpgradeLevel(FGameplayTag UpgradeTag);

	void Auth_IncrementUpgradeEffect(FGameplayTag UpgradeTag);

private:
	ETeam Team = ETeam::Neutral;


};
