// Copyright ZH_Dev


#include "UI/OverlayWidgetController.h"

#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/SamPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	OnHealthChangedDelegate.Broadcast(SamAS->GetHealth(), SamAS->GetMaxHealth(), SamAS->GetHealth()/SamAS->GetMaxHealth());
	
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	ULevelUpInfo* LevelUpInfo = SamGS->LevelUpInfo;
	
	FExpProgressDetails ExpDetails = LevelUpInfo->GetExpProgressDetails(SamPS->GetTotalExp());
	OnExpProgressChangedDelegate.Broadcast(ExpDetails.CurrentExp, ExpDetails.NeededExp, ExpDetails.ProgressPercentage);
	OnLevelChangedDelegate.Broadcast(SamPS->GetLevel());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::OnHealthChanged);

	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);

	//Gamestate is ready, otherwise, wait for it to replicate.
	if(SamGS)
		UE_LOG(SamLog, Warning, TEXT("Success"))
	SamGS->ExpChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChanged);
	SamGS->LevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::OnMaxHealthChanged);
	
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
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	ULevelUpInfo* LevelUpInfo = SamGS->LevelUpInfo;
	
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


