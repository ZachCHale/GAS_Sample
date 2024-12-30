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
	OnPlayerLobbyReadyCountChangedDelegate.Broadcast(0, SamGS->PlayerArray.Num());
	//Game Time only syncs the initial value, we don't bind in BindCallbacksToDependencies
	OnGameTimerSyncDelegate.Broadcast(SamGS->GetMatchTimeRemaining());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	SamPS->OnPlayerCharacterDeathDelegate.AddUObject(this, &ThisClass::OnPlayerCharacterDeath);
	SamPS->OnPlayerCharacterReviveDelegate.AddUObject(this, &ThisClass::OnPlayerCharacterRevive);

	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	
	SamGS->ExpChangedDelegate.AddUObject(this, &ThisClass::OnExpChanged);
	SamGS->LevelChangedDelegate.AddUObject(this, &ThisClass::OnLevelChanged);
	SamGS->BeginLevelUpSelectionDelegate.AddUObject(this, &ThisClass::OnBeginLevelUpSelection);
	SamGS->EndLevelUpSelectionDelegate.AddUObject(this, &ThisClass::OnEndLevelUpSelection);
	SamGS->PlayerUpgradeReadyCountChangedDelegate.AddUObject(this, &ThisClass::OnPlayerUpgradeReadyCountChanged);
	SamGS->PlayerLobbyReadyCountChangedDelegate.AddUObject(this, &ThisClass::OnPlayerLobbyReadyCountChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
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
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel) const
{
	OnLevelChangedDelegate.Broadcast(NewLevel);
}

void UOverlayWidgetController::OnBeginLevelUpSelection() const
{
	OnBeginLevelUpSelectionDelegate.Broadcast();
}

void UOverlayWidgetController::OnEndLevelUpSelection() const
{
	OnEndLevelUpSelectionDelegate.Broadcast();
}

void UOverlayWidgetController::OnPlayerUpgradeReadyCountChanged(int32 NewReadyCount, int32 NewTotalPlayerCount) const
{
	OnPlayerUpgradeReadyCountChangedDelegate.Broadcast(NewReadyCount, NewTotalPlayerCount);
}

void UOverlayWidgetController::OnPlayerLobbyReadyCountChanged(int32 NewReadyCount, int32 NewTotalPlayerCount) const
{
	OnPlayerLobbyReadyCountChangedDelegate.Broadcast(NewReadyCount, NewTotalPlayerCount);
}

void UOverlayWidgetController::OnPlayerCharacterDeath(ASamPlayerState*) const
{
	OnPlayerCharacterDeathDelegate.Broadcast();
}

void UOverlayWidgetController::OnPlayerCharacterRevive(ASamPlayerState*) const
{
	OnPlayerCharacterReviveDelegate.Broadcast();
}


