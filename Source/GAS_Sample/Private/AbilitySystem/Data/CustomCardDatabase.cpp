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
	const UCustomCardInfo* CustomCard = CustomCards[CardTag];
	TitleTextBock->SetText(CustomCard->CardTitle);
	IconImage->SetBrushFromTexture(CustomCard->IconImage);
	
	UUserWidget* Body = CreateWidget<UUserWidget>(BodyContainer, CardBodyClass);
	Cast<UCardBody_Custom>(Body)->SetInfo(CustomCard);
	BodyContainer->AddChild(Body);
}

void UCustomCardDatabase::ExecuteCustomCard(APlayerState* TargetPlayerState, FGameplayTag ActionTag)
{
	
	checkf(CustomCards.Contains(ActionTag), TEXT("Could not find Custom Card with associated GameplayTag : %s"), *ActionTag.ToString());
	CustomCards[ActionTag]->ExecuteCard(TargetPlayerState, ActionTag);
}
