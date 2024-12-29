// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "CardInfo.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SpecialFunctionCardAsset.generated.h"


class UCardBody_Special;
class UExecCardAction;
class UActionableCard;
class USpecialFunctionCard;

class USpecialFunctionCardAsset;

typedef void (USpecialFunctionCardAsset::*SpecialCardFuncPtr)(APlayerState*, FGameplayTag);

USTRUCT(BlueprintType)
struct FSpecialExecCardInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText CardTitle = FText::FromString("No Title");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description = FText::FromString("No Description");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconImage;
	FGameplayTag ExecCardTag = FGameplayTag();
	SpecialCardFuncPtr CardAction = nullptr;
};



/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USpecialFunctionCardAsset : public UDataAsset, public ICardInfo
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCardBody_Special> CardBodyClass;

	//~ICardInfoInterface
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End ICardInfoInterface
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FSpecialExecCardInfo> SpecialCards;
	
	// Finds the card with given tag and executes on the function pointer contained within.
	void ExecuteSpecialCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag);
	
	// Special Function for reviving the TargetPlayer.
	void RevivePlayer(APlayerState* TargetPlayer, FGameplayTag ActionTag);

private:
	void InitSpecialCardFunctions();

	// Adds function pointer to card with the same GameplayTag;
	void GiveFuncPtrToCard(FGameplayTag ExecCardTag, SpecialCardFuncPtr CardAction);

	// Checks if any cards in the DataAsset are missing a function to associate with.
	void CheckForMissingFunctions();

	bool bIsInitialized = false;

	
};
