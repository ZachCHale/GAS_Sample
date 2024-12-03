// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "LevelSpawnPatternInfo.generated.h"




USTRUCT(BlueprintType)
struct FEnemySpawnPatternInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "CharacterClass.Enemy"))
	FGameplayTag CharacterClassTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat MaxSpawnOverTime;
	int32 GetUnitCapAtTime(const float TimeSeconds) const
	{
		return MaxSpawnOverTime.AsInteger(TimeSeconds);
	}
};


USTRUCT(BlueprintType)
struct FSpawnPatternInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FEnemySpawnPatternInfo> EnemySpawnPatterns;
	
};

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ULevelSpawnPatternInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Level.Gameplay"))
	TMap<FGameplayTag, FSpawnPatternInfo> LevelSpawnPatternInfo;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FSpawnPatternInfo GetSpawnPatternInfoFromTag(FGameplayTag LevelTag);
	
};
