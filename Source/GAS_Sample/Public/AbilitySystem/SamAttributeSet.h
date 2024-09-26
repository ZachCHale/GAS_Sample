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

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USamAttributeSet();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Primary Attributes")  
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(USamAttributeSet, MovementSpeed);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TMap<FGameplayTag, FAttributeFuncPtr> TagsToAttributes;
	
};
