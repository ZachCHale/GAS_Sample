// Copyright ZH_Dev


#include "AbilitySystem/Data/UpgradeCardDatabase.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Player/SamPlayerState.h"
#include "UI/CardBody/CardBody_Upgrade.h"


void UUpgradeCardDatabase::InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag,
	UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer)
{
	if(!bIsInitialized)
		InitData();
	
	check(UpgradeCards.Contains(CardTag));
	const UUpgradeCardInfo* CardInfo = UpgradeCards[CardTag];
	ASamPlayerState* SamPS = Cast<ASamPlayerState>(TargetPlayer);
	USamAbilitySystemComponent* SamASC = Cast<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
	int32 CurrentLevel = SamASC->GetCurrentUpgradeLevel(CardTag);
	int32 NextLevel = CurrentLevel+1;
	FString LeveledTitle = FString::Printf(TEXT("%s %d"), *CardInfo->Title.ToString(), NextLevel);
	TitleTextBock->SetText(FText::FromString(LeveledTitle));
	IconImage->SetBrushFromTexture(CardInfo->IconImage);
	UUserWidget* Body = CreateWidget<UUserWidget>(BodyContainer, CardBodyClass);
	Cast<UCardBody_Upgrade>(Body)->SetInfo(CardInfo);
	BodyContainer->AddChild(Body);
	
}

void UUpgradeCardDatabase::ApplyUpgrade(APlayerState* TargetPlayer, FGameplayTag UpgradeTag)
{
	if(!bIsInitialized)
		InitData();
	
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(TargetPlayer);
	USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
	SamASC->Auth_IncrementUpgradeEffect(UpgradeTag);
}

TSubclassOf<UGameplayEffect> UUpgradeCardDatabase::GetEffectFromTag(FGameplayTag UpgradeTag)
{
	check(UpgradeCards.Contains(UpgradeTag));
	return UpgradeCards[UpgradeTag]->UpgradeEffect;
}



void UUpgradeCardDatabase::InitData()
{
	if(bIsInitialized)
		return;
	
	for (auto Card : UpgradeCards)
	{
		Card.Value->UpgradeTag = Card.Key;
	}
	bIsInitialized = true;
}

