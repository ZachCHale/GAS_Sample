// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "InitCardDisplay.h"
#include "GameplayTagContainer.h"
#include "Components/PanelWidget.h"
#include "Engine/DataAsset.h"
#include "CustomCardDatabase.generated.h"


class UCardBody_Custom;
class UExecCardAction;
class UActionableCard;
class USpecialFunctionCard;

class UCustomCardDatabase;

typedef void (UCustomCardDatabase::*SpecialCardFuncPtr)(APlayerState*, FGameplayTag);

UCLASS(BlueprintType, DefaultToInstanced, Blueprintable, EditInlineNew, Abstract)
class UCustomCardInfo : public UObject
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
 *	Cards that have custom functionality implementable via blueprint.
 */
UCLASS()
class GAS_SAMPLE_API UCustomCardDatabase : public UDataAsset, public IInitCardDisplay
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCardBody_Custom> CardBodyClass;

	//~IInitCardDisplay
	virtual void InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag, UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer) override;
	//~End IInitCardDisplay
	
	UPROPERTY(EditDefaultsOnly, Instanced)
	TMap<FGameplayTag, UCustomCardInfo*> CustomCards;
	
	// Finds the card with given tag and executes it
	void ExecuteCustomCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag);

	
};
