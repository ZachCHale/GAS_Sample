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

	FGameplayTag Sample_Test_Tag;
      
protected:  
private:  
	static FSamGameplayTags GameplayTags;  
};
