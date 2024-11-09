// Copyright ZH_Dev


#include "UI/HUD/SamHUD.h"

#include "SamLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "UI/OverlayWidgetController.h"
#include "UI/SamUserWidget.h"

UOverlayWidgetController* ASamHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

void ASamHUD::InitOverlay(const FWidgetControllerParams& WCParams)
{
	checkf(OverlayClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_SamHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_SamHUD"));

	if(AGameStateBase* GS = GetWorld()->GetGameState())
	{
		FinishInitOverlay(WCParams, GS);
	}else
	{
		GetWorld()->GameStateSetEvent.AddLambda([this, WCParams](AGameStateBase* GS){
			FinishInitOverlay(WCParams, GS);
		});
	}
}

void ASamHUD::FinishInitOverlay(const FWidgetControllerParams& WCParams, const AGameStateBase* GameState)
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayClass);
	OverlayWidget = Cast<USamUserWidget>(Widget);
	
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WCParams);

	OverlayWidget->SetWidgetController(WidgetController);

	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
}



