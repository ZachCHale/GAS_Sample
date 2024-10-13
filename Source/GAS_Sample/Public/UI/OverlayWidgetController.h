// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "SamWidgetControllerEvents.h"
#include "Player/SamPlayerState.h"
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
	FOnProgressBarStatChangedSignature OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnProgressBarStatChangedSignature OnExpProgressChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnIntStatChangedSignature OnLevelChangedDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

private:

	void OnHealthChanged(const FOnAttributeChangeData& NewHealth) const;
	void OnMaxHealthChanged(const FOnAttributeChangeData& NewMaxHealth) const;
	void OnExpChanged(int32 NewExp) const;
	void OnLevelChanged(int32 NewLevel) const;
};
