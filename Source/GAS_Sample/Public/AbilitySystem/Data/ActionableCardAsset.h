// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "CardInfo.h"
#include "GameplayTagContainer.h"
#include "UpgradeCardAsset.h"
#include "Engine/DataAsset.h"
#include "ActionableCardAsset.generated.h"

class USpecialFunctionCardAsset;
class USpecialFunctionCard;
//	Manages ALL Actionable Cards, Regardless of Type. Each Type should also have its own DataAsset, which will be referenced here.
UCLASS()
class GAS_SAMPLE_API UActionableCardAsset : public UDataAsset, public ICardInfo
{
	GENERATED_BODY()

public:

	//~ICardInfoInterface
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End ICardInfoInterface
	
	UPROPERTY(EditDefaultsOnly, Category="Universal")
	TObjectPtr<USpecialFunctionCardAsset> SpecialExecCardAsset;

	UPROPERTY(EditDefaultsOnly, Category="Upgrades")
	TObjectPtr<UUpgradeCardAsset> UpgradeCardAsset;

	void ExecuteCard(APlayerState* TargetPlayer, FGameplayTag ExecCardTag);

	TSubclassOf<UGameplayEffect> GetGameplayEffectFromUpgradeTag(FGameplayTag UpgradeTag) const;
};
