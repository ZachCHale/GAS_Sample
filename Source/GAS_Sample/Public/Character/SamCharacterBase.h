// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Actor/Interface/TeamInterface.h"
#include "SamCharacterBase.generated.h"

struct FCharacterClassDefaultInfo;
enum class ECharacterClass : uint8;
struct FOnAttributeChangeData;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ASamCharacterBase*, CharacterInstance);

UCLASS(Abstract)
class GAS_SAMPLE_API ASamCharacterBase : public ACharacter, public IAbilitySystemInterface, public ITeamInterface
{
	GENERATED_BODY()

public:
	ASamCharacterBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Auth_Die();

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeathDelegate;

	virtual ETeam GetTeam() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsDead() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsActive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetCharacterClassTag(){ return CharacterClassTag; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterClassDefaultInfo GetCharacterClassDefaultInfo();

	UFUNCTION(BlueprintCallable)
	void Auth_ActivateCharacter();

	UFUNCTION(BlueprintCallable)
	void Auth_DeactivateCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo();

	virtual void BindToAttributeChanges();

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data);

	void Auth_SetMovementSpeed(float NewSpeed);

	void Auth_InitDefaultAttributes();

	void Auth_InitDefaultAbilities();

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CharacterClassTag;

	bool bIsDead = false;

	//Character is not hidden in game, has collision, moves, and activates abilities if AI Controlled.
	bool bIsActive = true;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastHandleDeath();

	UPROPERTY(ReplicatedUsing = OnRep_MovementSpeed)
	float MovementSpeed;
	
	UFUNCTION()
	void OnRep_MovementSpeed() const;
};
