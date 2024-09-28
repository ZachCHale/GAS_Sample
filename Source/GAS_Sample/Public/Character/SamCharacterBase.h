// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SamCharacterBase.generated.h"

enum class ECharacterClass : uint8;
struct FOnAttributeChangeData;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(Abstract)
class GAS_SAMPLE_API ASamCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASamCharacterBase();
	
	void Die();

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeathDelegate;


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

	void SetMovementSpeed(float NewSpeed);

	void InitDefaultAttributes();

	UPROPERTY(EditDefaultsOnly)
	ECharacterClass CharacterClass;

	bool bIsDead = false;
	
};
