// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "SamWidgetControllerEvents.h"
#include "UI/SamWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;

/**
 * 
 */
UCLASS(BlueprintType)
class GAS_SAMPLE_API UOverlayWidgetController : public USamWidgetController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
};
