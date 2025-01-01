// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "InitCardDisplay.h"
#include "GameplayTagContainer.h"
#include "UpgradeCardDatabase.h"
#include "Engine/DataAsset.h"
#include "CardDatabase.generated.h"

class UCustomCardDatabase;
class USpecialFunctionCard;
//	Manages ALL Actionable Cards, Regardless of Type. Each Type should also have its own DataAsset, which will be referenced here.
UCLASS()
class GAS_SAMPLE_API UCardDatabase : public UDataAsset, public IInitCardDisplay
{
	GENERATED_BODY()

public:

	//~IInitCardDisplay
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End IInitCardDisplay
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCustomCardDatabase> CustomCardDatabase;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UUpgradeCardDatabase> UpgradeCardDatabase;

	void ExecuteCard(APlayerState* TargetPlayer, FGameplayTag ExecCardTag);

	TSubclassOf<UGameplayEffect> GetGameplayEffectFromUpgradeTag(FGameplayTag UpgradeTag) const;
};
