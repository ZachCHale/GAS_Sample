// Copyright ZH_Dev


#include "UI/SamWidgetController.h"

#include "AbilitySystem/SamAttributeSet.h"

void USamWidgetController::BroadcastInitialValues()
{

}

void USamWidgetController::BindCallbacksToDependencies()
{
}

void USamWidgetController::SetWidgetControllerParams(const FWidgetControllerParams WCParams)
{
	PlayerController = WCParams.PlayerController;  
	PlayerState = WCParams.PlayerState;  
	AbilitySystemComponent = WCParams.AbilitySystemComponent;  
	AttributeSet = WCParams.AttributeSet;  
}
