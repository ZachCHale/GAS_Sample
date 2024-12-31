// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"


USTRUCT(BlueprintType)
struct FExpProgressDetails
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentExp = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NeededExp = 0;
	UPROPERTY(BlueprintReadOnly)
	float ProgressPercentage = 0;
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
	FScalableFloat ExpRequirementPerLevel;

	int32 FindLevelFromTotalExp(int32 TotalExp);

	FExpProgressDetails GetExpProgressDetails(int32 TotalExp);
};
