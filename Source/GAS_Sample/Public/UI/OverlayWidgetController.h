// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "SamGameStateBase.h"
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
	FOnProgressBarStatChangedSignature_Dynamic OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnProgressBarStatChangedSignature_Dynamic OnExpProgressChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnIntStatChangedSignature_Dynamic OnLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnLevelUpSelectionSignature_Dynamic OnBeginLevelUpSelectionDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnLevelUpSelectionSignature_Dynamic OnEndLevelUpSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerCharacterDeathSignature_Dynamic OnPlayerCharacterDeathDelegate;

	// This will NOT be sent every tick that the time changes on the server.
	// After receiving this delegate, decrementing the time should be done locally for ui representation only.
	UPROPERTY(BlueprintAssignable)
	FOnFloatStatChangedSignature_Dynamic OnGameTimerSyncDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerReadyCountChangedSignature_Dynamic OnPlayerReadyCountChangedDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

private:

	void OnHealthChanged(const FOnAttributeChangeData& NewHealth) const;
	void OnMaxHealthChanged(const FOnAttributeChangeData& NewMaxHealth) const;
	void OnExpChanged(int32 NewExp) const;
	void OnLevelChanged(int32 NewLevel) const;
	void OnBeginLevelUpSelection() const;
	void OnEndLevelUpSelection() const;
	void OnPlayerReadyCountChanged(int32 NewReadyCount, int32 NewTotalPlayerCount) const;
	void OnPlayerCharacterDeath(ASamPlayerState*) const;
};
