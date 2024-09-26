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

	FGameplayTag Attribute_Primary_MovementSpeed;
      
protected:  
private:  
	static FSamGameplayTags GameplayTags;  
};
