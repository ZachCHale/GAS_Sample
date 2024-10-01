// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/Data/SamInputConfig.h"
#include "SamInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template <class UserClass, class PressedFuncType, class ReleasedFuncType, class HeldFuncType>
	void BindAbilityActions(const USamInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
	                        ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, class PressedFuncType, class ReleasedFuncType, class HeldFuncType>
void USamInputComponent::BindAbilityActions(const USamInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);  
	for(const FInputActionTagBinding& ActionBinding : InputConfig->InputActopTagBindings)  
	{  
		if(ActionBinding.InputAction && ActionBinding.GameplayTag.IsValid())  
		{
			if(PressedFunc)  
			{  
				BindAction(ActionBinding.InputAction, ETriggerEvent::Started, Object, PressedFunc, ActionBinding.GameplayTag);  
			}  
			if(HeldFunc)  
			{  
				BindAction(ActionBinding.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, ActionBinding.GameplayTag);  
			}  
			if(ReleasedFunc)  
			{  
				BindAction(ActionBinding.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, ActionBinding.GameplayTag);  
			}  
		}  
	}  
}
