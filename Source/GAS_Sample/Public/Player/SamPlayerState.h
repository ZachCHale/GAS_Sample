// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ExpLevelInterface.h"

#include "SamPlayerState.generated.h"

class ASamCharacterBase;
class ULevelUpInfo;
class ASamCharacterPlayer;
class UAttributeSet;
class ASamPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateEventSignature, ASamPlayerState*)


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
	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;
	//~END IExpLevelInterface
	
	UAttributeSet* GetAttributeSet() const;
	FPlayerStateEventSignature OnPlayerCharacterDeathDelegate;

	// Checks if the player has a character (ASamCharacterBase), and if so, checks that it isn't in a dying/dead state
	bool HasLivingCharacter() const;

	// Mostly handles binding to events on the character such as death.
	void InitWithPlayerCharacter(ASamCharacterPlayer* PlayerCharacter);

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	//TODO: Might be able to bind to the character directly from the controller. There isn't much reason to do it heer anymore.
	UFUNCTION()
	void HandleCharacterDeath(ASamCharacterBase* CharacterInstance);

};
