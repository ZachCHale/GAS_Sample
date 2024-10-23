// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "SamAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

typedef FGameplayAttribute(*FAttributeFuncPtr)();

UCLASS()
class GAS_SAMPLE_API USamAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	USamAttributeSet();
	
	/*
	 *	Primary Attributes
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Primary Attributes")  
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, MovementSpeed);
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;

	/*
	 *	Vital Attributes
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")  
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")  
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, Health);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	
	/*
	 *	Attack Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageScale, Category = "Attack Attributes")  
	FGameplayAttributeData DamageScale;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, DamageScale);
	UFUNCTION()
	void OnRep_DamageScale(const FGameplayAttributeData& OldDamageScale) const;

	/*
	 *	Resistance Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Resistance Attributes")  
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, PhysicalResistance);
	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicResistance, Category = "Resistance Attributes")  
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, MagicResistance);
	UFUNCTION()
	void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance) const;
	
	/*
	 *	Meta Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")  
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")  
	FGameplayAttributeData IncomingExp;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, IncomingExp)
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TMap<FGameplayTag, FAttributeFuncPtr> TagsToAttributes;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	void PostExecuteIncomingDamage(const FGameplayEffectModCallbackData& Data);
	void PostExecuteIncomingExp(const FGameplayEffectModCallbackData& Data);
};
