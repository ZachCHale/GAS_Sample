// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/UpgradeInfo.h"
#include "GameFramework/GameStateBase.h"
#include "Player/ExpLevelInterface.h"
#include "SamGameStateBase.generated.h"

class ULevelSpawnPatternInfo;
class ULevelUpInfo;
class ASamPlayerState;

UENUM()
enum EGameStateStatus: int32
{
	PreGameLobby,
	LevelUpSelection,
	Gameplay,
	GameEnd,
	PausedByPlayer,
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateEventSignature);

//TODO: See what can be moved into the player state. Try to better organize information about players relevant to the state of the game.

UCLASS()
class GAS_SAMPLE_API ASamGameStateBase : public AGameStateBase , public IExpLevelInterface
{
	GENERATED_BODY()

public:
	ASamGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~End AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	//~End AGameStateBase interface

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Auth_Tick(float DeltaSeconds);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UUpgradeInfo> UpgradeInfo;
	
	FOnGameStatChangedSignature ExpChangedDelegate;
	FOnGameStatChangedSignature LevelChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature NewGameStartDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature GameWonDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature GameLostDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature GamePausedByPlayerDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature GameUnpausedByPlayerDelegate;
	
	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };

	TArray<ACharacter*> GetAllPlayerCharacters();

	//Authority will always be accurate, clients will be when the GameState last replicated
	float GetLastSyncedGameTime() const;

	float Auth_GetCurrentGameProgress();

	/*
	 *	IExpLevelInterface
	 */
	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;
	/*
	 *	End IExpLevelInterface
	 */

	UFUNCTION(BlueprintCallable)
	EGameStateStatus GetGameStatus() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartNewGame();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameWon();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameLost();

	//Notify Clients to show pause menus.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GamePausedByPlayer();
	//Notify Clients to hide pause menus.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameUnpausedByPlayer();

	//Specifically for entering a paused game state that will display a pause menu.
	void Auth_PauseGame();
	//Specifically for exiting a paused game state that displayed a pause menu.
	void Auth_UnpauseGame();

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel() const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp() const;
	
	EGameStateStatus StateStatus = EGameStateStatus::PreGameLobby;
	
	const float MatchLengthSeconds = 600.f;
	float CurrentGameTimerValueSeconds;

	void HandlePlayerDeath(ASamPlayerState* PlayerState);

	void Auth_EndGameLoss();

	void Auth_EndGameWin();



	
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
	void Auth_SetPlayerReadyInLobby(const APlayerState* PlayerState);

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
