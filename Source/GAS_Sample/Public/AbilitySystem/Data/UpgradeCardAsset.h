// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "CardInfo.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "UpgradeCardAsset.generated.h"


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
	TArray<FUpgradeCardStatInfo> StatInfo;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag UpgradeTag = FGameplayTag();
};





UCLASS()
class GAS_SAMPLE_API UUpgradeCardAsset : public UDataAsset, public ICardInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCardBody_Upgrade> CardBodyClass;

	//~ICardInfoInterface
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End ICardInfoInterface
	
	void ApplyUpgrade(APlayerState* TargetPlayer, FGameplayTag UpgradeTag);

	//Gameplay Tag should be present on the Gameplay effect for the upgrade. TODO: Add a check/test for this, or automate it somehow.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TMap<FGameplayTag, UUpgradeCardInfo*> UpgradeCards;

	TSubclassOf<UGameplayEffect> GetEffectFromTag(FGameplayTag UpgradeTag);


private:
	bool bIsInitialized = false;

	void InitData();
};
