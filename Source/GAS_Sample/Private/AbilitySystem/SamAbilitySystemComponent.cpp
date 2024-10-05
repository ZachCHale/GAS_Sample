// Copyright ZH_Dev


#include "AbilitySystem/SamAbilitySystemComponent.h"

void USamAbilitySystemComponent::AbilityInputHeld(FGameplayTag GameplayTag)
{
	if(!GameplayTag.IsValid()) return;  
  
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  
	{  
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(GameplayTag))  
		{  
			AbilitySpecInputPressed(AbilitySpec);  
			if(!AbilitySpec.IsActive())  
			{  
				TryActivateAbility(AbilitySpec.Handle);  
			}  
		}  
	}  
}

void USamAbilitySystemComponent::AbilityInputReleased(FGameplayTag GameplayTag)
{
	if(!GameplayTag.IsValid()) return;  
  
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  
	{  
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(GameplayTag))  
		{  
			AbilitySpecInputReleased(AbilitySpec);  
		}  
	}  
}

ETeam USamAbilitySystemComponent::GetTeam()
{
	return Team;
}

void USamAbilitySystemComponent::SetTeam(ETeam InTeam)
{
	Team = InTeam;
}
