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
	const USpecialCardInfo* SpecialCard = SpecialCards[CardTag];
	TitleTextBock->SetText(SpecialCard->CardTitle);
	IconImage->SetBrushFromTexture(SpecialCard->IconImage);
	
	UUserWidget* Body = CreateWidget<UUserWidget>(BodyContainer, CardBodyClass);
	Cast<UCardBody_Special>(Body)->SetInfo(SpecialCard);
	BodyContainer->AddChild(Body);
}

void USpecialFunctionCardAsset::ExecuteSpecialCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag)
{
	if(!bIsInitialized)
		InitSpecialCardFunctions();
	
	checkf(SpecialCards.Contains(ActionTag), TEXT("Could not find Special Card with associated GameplayTag : %s"), *ActionTag.ToString());
	SpecialCards[ActionTag]->ExecuteCard(TargetPlayerState, ActionTag);
}



void USpecialFunctionCardAsset::InitSpecialCardFunctions()
{
	if(bIsInitialized) return;
	
	// Set the gameplay tags for the cards based on the keys used in the TMap.
	for (auto Card : SpecialCards)
	{
		Card.Value->ExecCardTag = Card.Key;
	}
	
	bIsInitialized = true;
}
