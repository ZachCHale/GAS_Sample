// Copyright ZH_Dev


#include "AbilitySystem/SamGameplayAbility.h"

#include "SamGameplayTags.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"

const FGameplayTagContainer* USamGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); 
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	
	return MutableTags;
}

void USamGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(SamASC->GetAttributeSet(USamAttributeSet::StaticClass()));

		float AttackSpeed = SamAS->GetAttackSpeed();
		float FinalCooldown = CooldownDuration;

		if(AttackSpeed !=0)
			FinalCooldown /= AttackSpeed;
		
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(SamTags::CallerMagnitudeTags::CallerMagnitude_AbilityCooldown, FinalCooldown);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
