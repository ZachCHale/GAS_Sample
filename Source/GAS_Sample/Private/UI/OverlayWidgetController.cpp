// Copyright ZH_Dev


#include "UI/OverlayWidgetController.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/SamPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);

	OnHealthChanged.Broadcast(SamAS->GetHealth());
	OnMaxHealthChanged.Broadcast(SamAS->GetMaxHealth());

	ULevelUpInfo* LevelUpInfo = SamPS->LevelUpInfo;
	FExperienceProgressDetails XPDetails = LevelUpInfo->GetExperienceProgressDetails(SamPS->GetXP());
	OnXPProgressChangedDelegate.Broadcast(XPDetails.CurrentExperience, XPDetails.NeededExperience, XPDetails.ProgressPercentage);
	OnLevelChangedDelegate.Broadcast(SamPS->GetLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});

	SamPS->ExperienceChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	SamPS->LevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	ULevelUpInfo* LevelUpInfo = SamPS->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find level up info"));
	FExperienceProgressDetails XPDetails = LevelUpInfo->GetExperienceProgressDetails(NewXP);

	OnXPProgressChangedDelegate.Broadcast(XPDetails.CurrentExperience, XPDetails.NeededExperience, XPDetails.ProgressPercentage);
	
	UE_LOG(SamLog, Log, TEXT("XP: %d / %d = %f"), XPDetails.CurrentExperience, XPDetails.NeededExperience, XPDetails.ProgressPercentage);
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel)
{
	OnLevelChangedDelegate.Broadcast(NewLevel);
	UE_LOG(SamLog, Log, TEXT("Level: %d"), NewLevel);
}


