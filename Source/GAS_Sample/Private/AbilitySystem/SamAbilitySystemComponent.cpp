// Copyright ZH_Dev


#include "AbilitySystem/SamAbilitySystemComponent.h"

#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"

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

void USamAbilitySystemComponent::TryActivateAbilitiesByDynamicTag(FGameplayTag GameplayTag)
{
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  
	{  
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(GameplayTag))  
		{  
			if(!AbilitySpec.IsActive())  
			{  
				TryActivateAbility(AbilitySpec.Handle);
			}  
		}  
	} 
}

int32 USamAbilitySystemComponent::GetCurrentUpgradeLevel(FGameplayTag UpgradeTag)
{
	TArray<FGameplayEffectSpec> ActiveEffectSpecs;
	ActiveGameplayEffects.GetAllActiveGameplayEffectSpecs(ActiveEffectSpecs);
	auto ActiveEffectHandles= GetActiveEffectsWithAllTags(FGameplayTagContainer(UpgradeTag));
	checkf(ActiveEffectHandles.Num() <= 1, TEXT("Found multiple active effects with the same Upgrade Tag: %s. Upgrade Tags should be unique to one gameplay effect class."), *UpgradeTag.GetTagName().ToString());
	if(ActiveEffectHandles.Num() == 1)
	{
		const FActiveGameplayEffect* FoundEffect = GetActiveGameplayEffect(ActiveEffectHandles[0]);
		return FoundEffect->Spec.GetLevel();
	}
	return 0;
}

void USamAbilitySystemComponent::Auth_IncrementUpgradeEffect(FGameplayTag UpgradeTag)
{
	TArray<FGameplayEffectSpec> ActiveEffectSpecs;
	ActiveGameplayEffects.GetAllActiveGameplayEffectSpecs(ActiveEffectSpecs);
	auto ActiveEffectHandles= GetActiveEffectsWithAllTags(FGameplayTagContainer(UpgradeTag));
	checkf(ActiveEffectHandles.Num() <= 1, TEXT("Found multiple active effects with the same Upgrade Tag. Upgrade Tags should be unique to one gameplay effect class."));
	if(ActiveEffectHandles.Num() == 1)
	{
		//Upgrade Existing Effect
		const FActiveGameplayEffect* FoundEffect = GetActiveGameplayEffect(ActiveEffectHandles[0]);
		int32 CurrentEffectLevel = FoundEffect->Spec.GetLevel();
		SetActiveGameplayEffectLevel(ActiveEffectHandles[0], CurrentEffectLevel + 1);
	}else
	{
		
		//Create New Effect at Level 1
		//UUpgradeInfo* UpgradeInfo = USamAbilitySystemLibrary::GetUpgradeInfo(this);
		//check(UpgradeInfo->GetUpgradeInfoFromTag(UpgradeTag).UpgradeEffect != nullptr)
		TSubclassOf<UGameplayEffect> UpgradeEffectClass = USamAbilitySystemLibrary::GetSamGameStateBase(this)->ExecCardAsset->GetGameplayEffectFromUpgradeTag(UpgradeTag);
		USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToSelf(GetOwnerActor(), UpgradeEffectClass, 1);
	}
}
