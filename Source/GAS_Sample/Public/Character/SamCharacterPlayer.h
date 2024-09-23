// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Character/SamCharacterBase.h"
#include "SamCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterPlayer : public ASamCharacterBase
{
	GENERATED_BODY()

	void InitAbilityActorInfo();

	virtual void OnRep_PlayerState() override;

	virtual void PossessedBy(AController* NewController) override;
	
};
