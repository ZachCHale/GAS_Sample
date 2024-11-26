// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/UpgradeInfo.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInterface.h"
#include "SamGameStateBase.generated.h"

class ULevelUpInfo;

UENUM()
enum EGameStateStatus: int32
{
	LevelUpSelection,
	Gameplay,
};

USTRUCT()
struct FPlayerLevelUpSelectionState
{
	GENERATED_BODY()
	FPlayerLevelUpSelectionState()
	{
		PlayerState = nullptr;
		bIsReady = false;
		CurrentlySelectedChoice = FGameplayTag();
	}
	FPlayerLevelUpSelectionState(APlayerState* NewPS)
	{
		PlayerState = NewPS;
		bIsReady = false;
		CurrentlySelectedChoice = FGameplayTag();
	}
	UPROPERTY()
	APlayerState* PlayerState;
	UPROPERTY()
	bool bIsReady;
	UPROPERTY()
	TArray<FGameplayTag> UpgradeChoiceTags;
	//Add some validation, just make sure Upgrade Choice tags contains this.
	UPROPERTY()
	FGameplayTag CurrentlySelectedChoice;
	void ResetSelectionState()
	{
		bIsReady = false;
		CurrentlySelectedChoice = FGameplayTag();
		UpgradeChoiceTags.Empty();
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStatChangedSignature, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyCountChangedSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE(FOnLevelupSelectionSignature);

UCLASS()
class GAS_SAMPLE_API ASamGameStateBase : public AGameStateBase , public IPlayerInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FOnGameStatChangedSignature ExpChangedDelegate;
	//This should be used for updating UI values that display the level.
	FOnGameStatChangedSignature LevelChangedDelegate;
	//This should be used specifically for opening and filling the level up screen.
	FOnLevelupSelectionSignature BeginLevelUpSelectionDelegate;
	FOnLevelupSelectionSignature EndLevelUpSelectionDelegate;

	FOnPlayerReadyCountChangedSignature PlayerReadyCountChangedDelegate;

	
	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };

	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForExp(int32 ExpValue) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UUpgradeInfo> UpgradeInfo;

	TArray<ACharacter*> GetAllPlayerCharacters();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartLevelUpEvent(int32 NewLevel, int32 NewExp, const TArray<FPlayerLevelUpSelectionState>& UpdatedLevelUpSelectionState);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndLevelUpEvent(int32 NewLevel);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateReadyCount(int32 NewReady, int32 NewTotal);


	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UFUNCTION()
	void Auth_ReadyUpPlayerForLevelUpSelection(const APlayerState* PlayerState);

	UFUNCTION()
	void Auth_SendPlayerLevelUpSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag);

	UFUNCTION()
	bool IsValidUpgradeSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag);

	UFUNCTION()
	void Auth_ClearPlayerLevelUpSelection(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	TArray<FUpgradeInfoItem> GetLocalPlayerSelectionChoices();

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel() const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp() const;
	
	UFUNCTION()  
	void OnRep_PlayerReadyCount() const;

	TArray<FPlayerLevelUpSelectionState> LevelUpSelectionState;

	void Auth_GenerateUpgradesForPlayers();

	EGameStateStatus StateStatus = EGameStateStatus::Gameplay;
	
	int32 PlayerReadyCount = 0;

	int32 QueuedLevelUps;
};
