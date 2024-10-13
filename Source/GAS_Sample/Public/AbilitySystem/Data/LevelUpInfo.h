// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpInfoItem
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ExpRequirement;
};

USTRUCT(BlueprintType)
struct FExpProgressDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentExp;
	UPROPERTY(BlueprintReadOnly)
	int32 NeededExp;
	UPROPERTY(BlueprintReadOnly)
	float ProgressPercentage;
};

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLevelUpInfoItem> LevelUpInfo;

	int32 FindLevelFromTotalExp(int32 TotalExp);

	FExpProgressDetails GetExpProgressDetails(int32 TotalExp);
};
