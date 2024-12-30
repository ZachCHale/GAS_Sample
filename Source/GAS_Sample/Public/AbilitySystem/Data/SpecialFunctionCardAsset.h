// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "CardInfo.h"
#include "GameplayTagContainer.h"
#include "Components/PanelWidget.h"
#include "Engine/DataAsset.h"
#include "SpecialFunctionCardAsset.generated.h"


class UCardBody_Special;
class UExecCardAction;
class UActionableCard;
class USpecialFunctionCard;

class USpecialFunctionCardAsset;

typedef void (USpecialFunctionCardAsset::*SpecialCardFuncPtr)(APlayerState*, FGameplayTag);

UCLASS(BlueprintType, DefaultToInstanced, Blueprintable, EditInlineNew, Abstract)
class USpecialCardInfo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText CardTitle = FText::FromString("No Title");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description = FText::FromString("No Description");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconImage;
	FGameplayTag ExecCardTag = FGameplayTag();
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteCard(APlayerState* TargetPlayer, FGameplayTag ActionTag);
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
	
	UPROPERTY(EditDefaultsOnly, Instanced)
	TMap<FGameplayTag, USpecialCardInfo*> SpecialCards;
	
	// Finds the card with given tag and executes it
	void ExecuteSpecialCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag);


private:
	void InitSpecialCardFunctions();

	bool bIsInitialized = false;

	
};
