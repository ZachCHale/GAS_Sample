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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;
	
	UAttributeSet* GetAttributeSet() const;

	virtual int32 GetLevel() override;
	virtual int32 GetTotalExp() override;

	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForExp(int32 ExpValue) override;

	FPlayerStateEventSignature OnPlayerCharacterDeathDelegate;

	bool HasLivingCharacter() const;

	void InitWithPlayerCharacter(ASamCharacterPlayer* PlayerCharacter);

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	static inline TArray<TObjectPtr<ASamPlayerState>> PlayerStateList;
	UFUNCTION()
	void HandleCharacterDeath(ASamCharacterBase* CharacterInstance);

};
