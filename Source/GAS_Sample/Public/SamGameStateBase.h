// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/UpgradeInfo.h"
#include "GameFramework/GameStateBase.h"
#include "Player/ExpLevelInterface.h"
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
class GAS_SAMPLE_API ASamGameStateBase : public AGameStateBase , public IExpLevelInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UUpgradeInfo> UpgradeInfo;
	
	FOnGameStatChangedSignature ExpChangedDelegate;
	FOnGameStatChangedSignature LevelChangedDelegate;
	
	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };

	TArray<ACharacter*> GetAllPlayerCharacters();

	/*
	 *	TODO: Change Name of PlayerInterface now that it is specifically for Levels and Experience.
	 *	IPlayerInterface
	 */
	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;
	/*
	 *	End IPlayerInterface
	 */

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel() const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp() const;

	EGameStateStatus StateStatus = EGameStateStatus::Gameplay;
	
	/*
	*	Upgrade Selection
	*/
	
public:
	FOnLevelupSelectionSignature BeginLevelUpSelectionDelegate;
	FOnLevelupSelectionSignature EndLevelUpSelectionDelegate;
	FOnPlayerReadyCountChangedSignature PlayerReadyCountChangedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartLevelUpEvent(int32 NewLevel, int32 NewExp, const TArray<FPlayerLevelUpSelectionState>& UpdatedLevelUpSelectionState);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndLevelUpEvent(int32 NewLevel);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateReadyCount(int32 NewReady, int32 NewTotal);

	UFUNCTION()
	void Auth_SendPlayerLevelUpSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag);

	UFUNCTION()
	bool IsValidUpgradeSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag);

	UFUNCTION()
	void Auth_ClearPlayerLevelUpSelection(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	TArray<FUpgradeInfoItem> GetLocalPlayerSelectionChoices();
	
private:

	TArray<FPlayerLevelUpSelectionState> LevelUpSelectionState;
	
	int32 PlayerReadyCount = 0;
	int32 QueuedLevelUps;

	
	UFUNCTION()  
	void OnRep_PlayerReadyCount() const;

	void Auth_GenerateUpgradesForPlayers();

	FPlayerLevelUpSelectionState* GetPlayerLevelUpSelectionState(const APlayerState* PlayerState);

	int32 CountPlayerSelectionsReady();
	
	void Auth_SubmitAllPlayerUpgradeSelections();
		
	/*
	*	END Upgrade Selection
	*/
};
