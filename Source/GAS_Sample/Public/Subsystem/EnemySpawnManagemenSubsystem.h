// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemySpawnManagemenSubsystem.generated.h"

class ASamCharacterBase;/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UEnemySpawnManagemenSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;
private:
	const float DelayFromGameStart = 3.f;
	const float SpawnAttemptFreq = 1.f;
	float TimeTillNextSpawnAttempt;

	void StartSpawnAttempt();
	TObjectPtr<UWorld> ScopedWorld;

	float SpawnDistance = 2000.f;
	float VerticalOffset = 51.f;

	TMap<FGameplayTag, int32> LiveUnitCount;

	UFUNCTION()
	void HandleUnitDeath(ASamCharacterBase* UnitInstance);

};
