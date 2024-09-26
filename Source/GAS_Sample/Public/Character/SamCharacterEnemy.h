// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Character/SamCharacterBase.h"
#include "SamCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterEnemy : public ASamCharacterBase
{
	GENERATED_BODY()
	ASamCharacterEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void MoveTowardsClosestPlayer();

protected:
	virtual void InitAbilityActorInfo() override;
};
