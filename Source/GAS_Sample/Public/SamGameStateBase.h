// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInterface.h"
#include "SamGameStateBase.generated.h"

class ULevelUpInfo;

USTRUCT()
struct FPlayerLevelUpSelectionState
{
	GENERATED_BODY()
	FPlayerLevelUpSelectionState()
	{
		PlayerState = nullptr;
		bIsReady = false;
		PlaceholderChoices.Init(0,3);
		CurrentlySelectedChoice = 0;
	}
	FPlayerLevelUpSelectionState(APlayerState* NewPS)
	{
		PlayerState = NewPS;
		bIsReady = false;
		PlaceholderChoices.Init(0,3);
		CurrentlySelectedChoice = 0;
	}
	APlayerState* PlayerState;
	bool bIsReady;
	TArray<int32> PlaceholderChoices;
	int32 CurrentlySelectedChoice;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStatChangedSignature, int32);
//TODO: Add a param representing choices that were generated on the server. Create a ServerRPC that sends the index of the choice made by the client.
//Where should choices be stored until client selects? We can keep all of them on the Player State or game state. Clients dont NEED to know about the other players choices, but there isn't any harm in it, and could be used later.
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
	
	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };

	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForExp(int32 ExpValue) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	TArray<ACharacter*> GetAllPlayerCharacters();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartLevelUpEvent(int32 NewLevel);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndLevelUpEvent(int32 NewLevel);

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UFUNCTION()
	void Auth_ReadyUpPlayerForLevelUpSelection(APlayerState* PlayerState);

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel(int32 OldLevel) const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp(int32 OldExp) const;

	TArray<FPlayerLevelUpSelectionState> LevelUpSelectionState;
	
};
