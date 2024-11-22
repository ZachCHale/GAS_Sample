// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Components/Image.h"
#include "Engine/DataAsset.h"
#include "UpgradeInfo.generated.h"

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
	
};

UCLASS()
class GAS_SAMPLE_API UUpgradeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUpgradeInfoItem> UpgradeInfo;

	FUpgradeInfoItem GetUpgradeInfoFromTag(FGameplayTag UpgradeTag) const;

	TArray<FGameplayTag> GetRandomUpgradeTags(int32 TagCount);
};
