// Copyright ZH_Dev


#include "AbilitySystem/Data/CardDatabase.h"

#include "SamGameplayTags.h"
#include "AbilitySystem/Data/CustomCardDatabase.h"

void UCardDatabase::InitializeCardDisplay(APlayerState* TargetPlayer, FGameplayTag CardTag,
                                          UTextBlock* TitleTextBock, UImage* IconImage, UPanelWidget* BodyContainer)
{
	IInitCardDisplay::InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	
	// Determine the type of card
	if(CardTag.MatchesTag(SamTags::Cards::Card_Special))
	{
		CustomCardDatabase->InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	}
	else if(CardTag.MatchesTag(SamTags::Cards::Card_Upgrade))
	{
		UpgradeCardDatabase->InitializeCardDisplay(TargetPlayer, CardTag, TitleTextBock, IconImage, BodyContainer);
	}
	
}

void UCardDatabase::ExecuteCard(APlayerState* TargetPlayer, FGameplayTag ExecCardTag)
{
	// Determine the type of card
	if(ExecCardTag.MatchesTag(SamTags::Cards::Card_Special))
	{
		CustomCardDatabase->ExecuteCustomCard(TargetPlayer, ExecCardTag);
	}
	else if(ExecCardTag.MatchesTag(SamTags::Cards::Card_Upgrade))
	{
		UpgradeCardDatabase->ApplyUpgrade(TargetPlayer, ExecCardTag);
	}
}

TSubclassOf<UGameplayEffect> UCardDatabase::GetGameplayEffectFromUpgradeTag(FGameplayTag UpgradeTag) const
{
	return UpgradeCardDatabase->GetEffectFromTag(UpgradeTag);
}
