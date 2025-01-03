// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "InitCardDisplay.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "UpgradeCardDatabase.generated.h"


class UGameplayEffect;
class UCardBody_Upgrade;


USTRUCT(BlueprintType)
struct FUpgradeCardStatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText StatName = FText::FromString("No Title");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDisplayAsPercentage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat Magnitude;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UUpgradeCardInfo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Title = FText::FromString("No Title");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> UpgradeEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelCap;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUpgradeCardStatInfo> StatInfo;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag UpgradeTag = FGameplayTag();
};


UCLASS()
class GAS_SAMPLE_API UUpgradeCardDatabase : public UDataAsset, public IInitCardDisplay
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCardBody_Upgrade> CardBodyClass;

	//~IInitCardDisplay
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End IInitCardDisplay
	
	void ApplyUpgrade(APlayerState* TargetPlayer, FGameplayTag UpgradeTag);

	//Gameplay Tag should be present on the Gameplay effect for the upgrade. TODO: Add a check/test for this, or automate it somehow.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TMap<FGameplayTag, UUpgradeCardInfo*> UpgradeCards;

	TSubclassOf<UGameplayEffect> GetEffectFromTag(FGameplayTag UpgradeTag);

	int32 GetCapForUpgrade(FGameplayTag UpgradeTag);


private:
	bool bIsInitialized = false;

	void InitData();
};
