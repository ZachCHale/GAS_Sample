// Copyright ZH_Dev


#include "AbilitySystem/Data/SamInputConfig.h"

const UInputAction* USamInputConfig::FindAbilityActionForTag(const FGameplayTag& GameplayTag)
{
	for (FInputActionTagBinding Binding : InputActopTagBindings)
	{
		if(Binding.GameplayTag.MatchesTagExact(GameplayTag))
		{
			return Binding.InputAction;
		}
	}
	return nullptr;
}
