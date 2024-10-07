// Copyright ZH_Dev
#pragma once

#include "CoreMinimal.h"  
#include "GameplayTagContainer.h"

/**  
 * SamGameplayTags
 * Singleton containing native Gameplay Tags
 */  
struct FSamGameplayTags  
{  
public:  
	static const FSamGameplayTags& Get(){ return GameplayTags;}  
	static void InitializeNativeGameplayTags();

	FGameplayTag Attribute_Vital_MaxHealth;
	FGameplayTag Attribute_Vital_Health;

	FGameplayTag Attribute_Primary_MovementSpeed;

	FGameplayTag InputBinding_SlottedAbility_1;

	FGameplayTag AbilityTag_ActivateOnDeath;

      
protected:  
private:  
	static FSamGameplayTags GameplayTags;  
};
