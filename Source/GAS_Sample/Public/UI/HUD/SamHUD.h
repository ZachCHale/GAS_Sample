// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SamHUD.generated.h"

class UAttributeSet;
class UOverlayWidgetController;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class USamUserWidget;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamHUD : public AHUD
{
	GENERATED_BODY()
public:

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(const FWidgetControllerParams& WCParams);


protected:
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USamUserWidget> OverlayClass;

	UPROPERTY()
	TObjectPtr<USamUserWidget> OverlayWidget;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	void FinishInitOverlay(const  FWidgetControllerParams& WCParams, const AGameStateBase* GameState);
	
};
