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

private:
	ETeam Team = ETeam::Neutral;

};
