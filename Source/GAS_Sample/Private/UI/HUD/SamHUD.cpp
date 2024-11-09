// Copyright ZH_Dev


#include "UI/HUD/SamHUD.h"

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
	if(b_IsInitialized) return;
	b_IsInitialized = true;
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayClass);
	OverlayWidget = Cast<USamUserWidget>(Widget);


	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WCParams);

	OverlayWidget->SetWidgetController(WidgetController);

	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
}
