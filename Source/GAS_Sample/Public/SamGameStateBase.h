// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/ActionableCardAsset.h"
#include "AbilitySystem/Data/UpgradeInfo.h"
#include "GameFramework/GameStateBase.h"
#include "Player/ExpLevelInterface.h"
#include "SamGameStateBase.generated.h"

class UCharacterClassInfo;
struct FPlayerUpgradeState;
class ULevelSpawnPatternInfo;
class ULevelUpInfo;
class ASamPlayerState;

UENUM()
enum class EGameStateStatus: int32
{
	PreGameLobby,
	LevelUpSelection,
	Gameplay,
	GameEnd,
	PausedByPlayer,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStatChangedSignature, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyCountChangedSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE(FOnLevelupSelectionSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateEventSignature_Dynamic);


UCLASS()
class GAS_SAMPLE_API ASamGameStateBase : public AGameStateBase , public IExpLevelInterface
{
	GENERATED_BODY()

public:
	ASamGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	//~End AGameStateBase interface

	//~IExpLevelInterface
	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;
	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };
	//~End IExpLevelInterface
	
	void Auth_Tick(float DeltaSeconds);

	//~Data Assets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UUpgradeInfo> UpgradeInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UActionableCardAsset> ExecCardAsset;
	//~End Data Assets

	//~Level/Exp Delegates
	FOnGameStatChangedSignature ExpChangedDelegate;
	FOnGameStatChangedSignature LevelChangedDelegate;
	//~End Level/Exp Delegates

	//~UpgradeSelection Delegates
	FOnLevelupSelectionSignature BeginLevelUpSelectionDelegate;
	FOnLevelupSelectionSignature EndLevelUpSelectionDelegate;
	FOnPlayerReadyCountChangedSignature PlayerUpgradeReadyCountChangedDelegate;
	FOnPlayerReadyCountChangedSignature PlayerLobbyReadyCountChangedDelegate;
	//~End UpgradeSelection Delegates

	//~GameState Delegates
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature_Dynamic NewGameStartDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature_Dynamic GameWonDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature_Dynamic GameLostDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature_Dynamic GamePausedByPlayerDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGameStateEventSignature_Dynamic GameUnpausedByPlayerDelegate;
	//~End GameState Delegates

	// Returns all player characters, even if in a dead/dying state.
	TArray<ACharacter*> GetAllPlayerCharacters();

	UFUNCTION(BlueprintCallable)
	float GetMatchTimeRemaining() const;

	//Returns the progress of the current in game time against the total match length, between 0.0 and 1.0
	float GetCurrentGameProgress();

	// Returns an ENUM representing the current state of the game.
	UFUNCTION(BlueprintCallable)
	EGameStateStatus GetGameStatus() const;

	// Starts new game and alerts clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartNewGame();

	// Alerts clients the game has been won.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameWon();

	// Alerts clients the game has been lost.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameLost();

	// Alert clients to show pause menus.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GamePausedByPlayer();
	
	// Alert Clients to hide pause menus.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameUnpausedByPlayer();

	//Specifically for entering a paused game state that will display a pause menu.
	void Auth_PauseGame();
	
	//Specifically for exiting a paused game state that displayed a pause menu.
	void Auth_UnpauseGame();

	UFUNCTION()
	void HandleAuthPlayerLobbyStateChanged(ASamPlayerState* PlayerState);

	// Replicates Level and Exp to Clients and Broadcasts BeginLevelUpSelectionDelegate
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartLevelUpEvent(int32 NewLevel, int32 NewExp);

	// Broadcasts EndLevelUpDelegate To Clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndLevelUpEvent(int32 NewLevel);

	// Replicates Ready Count To Clients And Broadcasts Delegate
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateUpgradeSelectionReadyCount(int32 NewReady, int32 NewTotal);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateLobbyReadyCount(int32 NewReady, int32 NewTotal);

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel() const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SyncMatchTime(float TimeRemaining);
	
	EGameStateStatus StateStatus = EGameStateStatus::PreGameLobby;
	
	const float MatchLengthSeconds = 600.f;
	float CurrentGameTimerValueSeconds;

	void HandlePlayerDeath(ASamPlayerState* PlayerState);

	//Ends the Game in a loss, then alerts clients
	void Auth_EndGameLoss();
	//Ends the Game in a win, then alerts clients
	void Auth_EndGameWin();
	
	// Allows handling multiple LevelUps occuring in a single frame
	int32 QueuedLevelUps;

	// Update Ready Count then Apply Upgrades if all players are ready.
	void HandleAuthPlayerUpgradeStateChanged(ASamPlayerState*);

	// Apply selected upgrades to players, then resume gameplay unless more level ups are queued.
	void Auth_ApplyAllPlayerUpgradeSelections();
	// Loop through the all player states and count how many are ready.
	int32 GetPlayerSelectionsReady();
	int32 GetPlayerLobbyReady();
};
