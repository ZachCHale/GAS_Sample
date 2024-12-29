// Copyright ZH_Dev


#include "AbilitySystem/Data/ActionableCardAsset.h"

#include "SamGameplayTags.h"
#include "AbilitySystem/Data/ExecCardAction.h"
#include "AbilitySystem/Data/SpecialFunctionCardAsset.h"

void UActionableCardAsset::InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag,
	UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer)
{
	ICardInfo::InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	
	// Determine the type of card
	if(CardTag.MatchesTag(SamTags::ExecCards::ExecCard_Special))
	{
		SpecialExecCardAsset->InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	}
	else if(CardTag.MatchesTag(SamTags::ExecCards::ExecCard_Upgrade))
	{
		UpgradeCardAsset->InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	}
	
}

void UActionableCardAsset::ExecuteCard(APlayerState* TargetPlayer, FGameplayTag ExecCardTag)
{
	// Determine the type of card
	if(ExecCardTag.MatchesTag(SamTags::ExecCards::ExecCard_Special))
	{
		SpecialExecCardAsset->ExecuteSpecialCard(TargetPlayer, ExecCardTag);
	}
	else if(ExecCardTag.MatchesTag(SamTags::ExecCards::ExecCard_Upgrade))
	{
		UpgradeCardAsset->ApplyUpgrade(TargetPlayer, ExecCardTag);
	}
}

TSubclassOf<UGameplayEffect> UActionableCardAsset::GetGameplayEffectFromUpgradeTag(FGameplayTag UpgradeTag) const
{
	return UpgradeCardAsset->GetEffectFromTag(UpgradeTag);
}
