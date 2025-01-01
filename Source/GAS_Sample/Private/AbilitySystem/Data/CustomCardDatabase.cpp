// Copyright ZH_Dev


#include "AbilitySystem/Data/CustomCardDatabase.h"

#include "SamGameplayTags.h"
#include "SamLogChannels.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "UI/CardBody/CardBody_Custom.h"


void UCustomCardDatabase::InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag,
                                                UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer)
{
	check(CustomCards.Contains(CardTag));
	const UCustomCardInfo* SpecialCard = CustomCards[CardTag];
	TitleTextBock->SetText(SpecialCard->CardTitle);
	IconImage->SetBrushFromTexture(SpecialCard->IconImage);
	
	UUserWidget* Body = CreateWidget<UUserWidget>(BodyContainer, CardBodyClass);
	Cast<UCardBody_Custom>(Body)->SetInfo(SpecialCard);
	BodyContainer->AddChild(Body);
}

void UCustomCardDatabase::ExecuteSpecialCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag)
{
	if(!bIsInitialized)
		InitSpecialCardFunctions();
	
	checkf(CustomCards.Contains(ActionTag), TEXT("Could not find Special Card with associated GameplayTag : %s"), *ActionTag.ToString());
	CustomCards[ActionTag]->ExecuteCard(TargetPlayerState, ActionTag);
}



void UCustomCardDatabase::InitSpecialCardFunctions()
{
	if(bIsInitialized) return;
	
	// Set the gameplay tags for the cards based on the keys used in the TMap.
	for (auto Card : CustomCards)
	{
		Card.Value->ExecCardTag = Card.Key;
	}
	
	bIsInitialized = true;
}
