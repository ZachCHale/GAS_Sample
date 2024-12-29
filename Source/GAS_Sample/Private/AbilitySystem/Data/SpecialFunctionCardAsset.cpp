// Copyright ZH_Dev


#include "AbilitySystem/Data/SpecialFunctionCardAsset.h"

#include "SamGameplayTags.h"
#include "SamLogChannels.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "UI/CardBody/CardBody_Special.h"


void USpecialFunctionCardAsset::InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag,
                                                      UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer)
{
	check(SpecialCards.Contains(CardTag));
	const FSpecialExecCardInfo* SpecialCard = &SpecialCards[CardTag];
	TitleTextBock->SetText(SpecialCard->CardTitle);
	IconImage->SetBrushFromTexture(SpecialCard->IconImage);
	
	UUserWidget* Body = CreateWidget<UUserWidget>(BodyContainer, CardBodyClass);
	Cast<UCardBody_Special>(Body)->SetInfo(*SpecialCard);
	BodyContainer->AddChild(Body);
}

void USpecialFunctionCardAsset::ExecuteSpecialCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag)
{
	if(!bIsInitialized)
		InitSpecialCardFunctions();
	
	checkf(SpecialCards.Contains(ActionTag), TEXT("Could not find Special Card with associated GameplayTag : %s"), *ActionTag.ToString());
	checkf(SpecialCards[ActionTag].CardAction != nullptr, TEXT("Special Card with associated GameplayTag is missing a function pointer : %s"), *ActionTag.ToString());
	
	(this->*(SpecialCards[ActionTag].CardAction))(TargetPlayerState, ActionTag);
}



void USpecialFunctionCardAsset::InitSpecialCardFunctions()
{
	if(bIsInitialized) return;
	
	// Set the gameplay tags for the cards based on the keys used in the TMap.
	for (auto Card : SpecialCards)
	{
		Card.Value.ExecCardTag = Card.Key;
	}

	// Assign Function Pointers for every card.
	GiveFuncPtrToCard(SamTags::ExecCards::ExecCard_Special_Revive, &ThisClass::RevivePlayer);

	CheckForMissingFunctions();
	bIsInitialized = true;
}

void USpecialFunctionCardAsset::GiveFuncPtrToCard(FGameplayTag ExecCardTag, SpecialCardFuncPtr CardAction)
{
	if(!SpecialCards.Contains(ExecCardTag))
	{
		UE_LOG(SamLog, Error, TEXT("Cannot add Special Function to ExecCard : %s. Please create a card with the associated tag within the DataAsset."), *ExecCardTag.ToString());
		return;
	}
	SpecialCards[ExecCardTag].CardAction = CardAction;
}

void USpecialFunctionCardAsset::CheckForMissingFunctions()
{
	for (auto Card : SpecialCards)
	{
		if(Card.Value.CardAction == nullptr)
		{
			UE_LOG(SamLog, Error, TEXT("Special Function Card is missing function : %s. Implement a function and call GiveFuncPtrToCard."), *Card.Value.ExecCardTag.ToString());
		}
	}
}

void USpecialFunctionCardAsset::RevivePlayer(APlayerState* TargetPlayer, FGameplayTag ActionTag)
{
	UE_LOG(SamLog, Log, TEXT("Attempting To Revive Player"));
}
