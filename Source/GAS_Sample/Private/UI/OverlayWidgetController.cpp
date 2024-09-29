// Copyright ZH_Dev


#include "UI/OverlayWidgetController.h"

#include "AbilitySystem/SamAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(SamAS->GetHealth());
	OnMaxHealthChanged.Broadcast(SamAS->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});
}


