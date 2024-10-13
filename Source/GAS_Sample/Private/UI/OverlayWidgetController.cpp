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
	FExpProgressDetails ExpDetails = LevelUpInfo->GetExpProgressDetails(SamPS->GetTotalExp());
	OnExpProgressChangedDelegate.Broadcast(ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
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

	SamPS->ExpChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChanged);
	SamPS->LevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
}

void UOverlayWidgetController::OnExpChanged(int32 NewExp)
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	ULevelUpInfo* LevelUpInfo = SamPS->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find level up info"));
	FExpProgressDetails ExpDetails = LevelUpInfo->GetExpProgressDetails(NewExp);

	OnExpProgressChangedDelegate.Broadcast(ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
	
	UE_LOG(SamLog, Log, TEXT("Exp: %d / %d = %f"), ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel)
{
	OnLevelChangedDelegate.Broadcast(NewLevel);
	UE_LOG(SamLog, Log, TEXT("Level: %d"), NewLevel);
}


