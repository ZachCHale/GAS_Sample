// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ExpLevelInterface.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

#include "SamPlayerState.generated.h"

struct FUpgradeInfoItem;
class ASamCharacterBase;
class ASamCharacterPlayer;
class UAttributeSet;
class ASamPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateEventSignature, ASamPlayerState*)

USTRUCT()
struct FPlayerUpgradeState
{
	GENERATED_BODY()
	FPlayerUpgradeState()
	{
		bIsReady = false;
		CurrentlySelectedCardTag = FGameplayTag();
	}
	void ResetSelectionState()
	{
		bIsReady = false;
		CurrentlySelectedCardTag = FGameplayTag();
		DrawnCardTags.Empty();
	}

	// The player has selected an upgrade.
	UPROPERTY()
	bool bIsReady;

	// Tags representing the upgrades available to choose from right now.
	UPROPERTY()
	TArray<FGameplayTag> DrawnCardTags;
	// The current upgrade the player has selected.
	UPROPERTY()
	FGameplayTag CurrentlySelectedCardTag;
};

USTRUCT()
struct FPlayerLobbyState
{
	GENERATED_BODY()
	FPlayerLobbyState()
	{
		bIsReady = false;
	}
	UPROPERTY()
	bool bIsReady;
};


UCLASS()
class GAS_SAMPLE_API ASamPlayerState : public APlayerState, public IAbilitySystemInterface, public IExpLevelInterface
{
	GENERATED_BODY()
public:
	ASamPlayerState();
	//~AActor interface
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	//~END AActor interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~END IAbilitySystemInterface

	//~IExpLevelInterface
	virtual int32 GetLevel() override;
	virtual int32 GetTotalExp() override;
	virtual void Auth_AddToExp(int32 AddedExp) override;
	virtual void Auth_AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;
	//~END IExpLevelInterface
	
	UAttributeSet* GetAttributeSet() const;
	
	FPlayerStateEventSignature OnPlayerCharacterDeathDelegate;
	FPlayerStateEventSignature OnPlayerCharacterReviveDelegate;
	FPlayerStateEventSignature OnAuthUpgradeStateChangedDelegate;
	FPlayerStateEventSignature OnAuthLobbyStateChangedDelegate;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> FullHealGameplayEffect;
	

	// Checks if the player has a character (ASamCharacterBase), and if so, checks that it isn't in a dying/dead state
	bool HasLivingCharacter() const;

	// Mostly handles binding to events on the character such as death.
	void InitWithPlayerCharacter(ASamCharacterPlayer* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	TArray<FGameplayTag> GetAvailableCardTags();

	FPlayerUpgradeState* GetPlayerUpgradeState();
	FPlayerLobbyState* GetPlayerLobbyState();

	UFUNCTION(BlueprintCallable)
	void Auth_ReviveCharacter();

	UFUNCTION(Server, Reliable)
	void Server_StartNewUpgradeState();

	void Auth_SubmitUpgradeSelection(FGameplayTag UpgradeTag);

	void Auth_ClearUpgradeSelection();

	void Auth_ReadyUpPlayerLobbyState();

	// True if UpgradeTag matches one of the choices in the Upgrade State
	bool IsUpgradeSelectionValid(FGameplayTag UpgradeTag) const;
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	UFUNCTION()
	void HandleCharacterDeath(ASamCharacterBase* CharacterInstance);
	UFUNCTION()
	void HandleCharacterRevive(ASamCharacterBase* CharacterInstance);

	FPlayerUpgradeState UpgradeState;
	FPlayerLobbyState LobbyState;

	// Called From Server_StartNewUpgradeState
	UFUNCTION(Client, Reliable)
	void Client_StartNewUpgradeState(FPlayerUpgradeState NewUpgradeState);

	// Indicates if a player is ready during the current game state (PreGameLobby, LevelUpSelection, etc)
	bool bIsReady;

};
