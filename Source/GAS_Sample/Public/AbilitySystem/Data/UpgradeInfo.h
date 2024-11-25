// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Components/Image.h"
#include "Engine/DataAsset.h"
#include "UpgradeInfo.generated.h"

USTRUCT(BlueprintType)
struct FStatChangePreview
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FText StatName;
	UPROPERTY(BlueprintReadOnly)
	float StartingValue;
	UPROPERTY(BlueprintReadOnly)
	float PredictedValue;
};

USTRUCT(BlueprintType)
struct FUpgradeInfoItem
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Title;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture> IconImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag UpgradeTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> UpgradeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText StatName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool PercentageFormat;
};



UCLASS()
class GAS_SAMPLE_API UUpgradeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUpgradeInfoItem> UpgradeInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCurveTable> UpgradeMagnitudeTable;

	FUpgradeInfoItem GetUpgradeInfoFromTag(FGameplayTag UpgradeTag) const;

	TArray<FGameplayTag> GetRandomUpgradeTags(int32 TagCount);

	UFUNCTION(BlueprintCallable)
	FStatChangePreview GetStatChangePreview(FGameplayTag UpgradeTag, int32 StartingLevel, int32 EndingLevel);

};
