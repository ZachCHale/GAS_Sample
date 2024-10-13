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
	OnHealthChangedDelegate.Broadcast(SamAS->GetHealth(), SamAS->GetMaxHealth(), SamAS->GetHealth()/SamAS->GetMaxHealth());

	ULevelUpInfo* LevelUpInfo = SamPS->LevelUpInfo;
	FExpProgressDetails ExpDetails = LevelUpInfo->GetExpProgressDetails(SamPS->GetTotalExp());
	OnExpProgressChangedDelegate.Broadcast(ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
	OnLevelChangedDelegate.Broadcast(SamPS->GetLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::OnHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::OnMaxHealthChanged);

	SamPS->ExpChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChanged);
	SamPS->LevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
}

void UOverlayWidgetController::OnHealthChanged(const FOnAttributeChangeData& NewHealth) const
{
	USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	OnHealthChangedDelegate.Broadcast(NewHealth.NewValue, SamAS->GetMaxHealth(), NewHealth.NewValue/SamAS->GetMaxHealth());
}

void UOverlayWidgetController::OnMaxHealthChanged(const FOnAttributeChangeData& NewMaxHealth) const
{
	USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	OnHealthChangedDelegate.Broadcast(SamAS->GetHealth(), NewMaxHealth.NewValue, SamAS->GetHealth()/NewMaxHealth.NewValue);
}

void UOverlayWidgetController::OnExpChanged(int32 NewExp) const
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	ULevelUpInfo* LevelUpInfo = SamPS->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find level up info"));
	FExpProgressDetails ExpDetails = LevelUpInfo->GetExpProgressDetails(NewExp);

	OnExpProgressChangedDelegate.Broadcast(ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
	
	UE_LOG(SamLog, Log, TEXT("Exp: %d / %d = %f"), ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel) const
{
	OnLevelChangedDelegate.Broadcast(NewLevel);
	UE_LOG(SamLog, Log, TEXT("Level: %d"), NewLevel);
}


