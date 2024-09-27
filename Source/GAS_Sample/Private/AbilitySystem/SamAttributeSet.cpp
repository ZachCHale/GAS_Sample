// Copyright ZH_Dev

#include "AbilitySystem/SamAttributeSet.h"

#include "SamGameplayTags.h"
#include "Net/UnrealNetwork.h"

USamAttributeSet::USamAttributeSet()
{
	const FSamGameplayTags& Tags = FSamGameplayTags::Get();

	TagsToAttributes.Add(Tags.Attribute_Primary_MovementSpeed, GetMovementSpeedAttribute);
	TagsToAttributes.Add(Tags.Attribute_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(Tags.Attribute_Vital_Health, GetHealthAttribute);
}

void USamAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,MovementSpeed,OldMovementSpeed);
}

void USamAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,MaxHealth,OldMaxHealth);
}

void USamAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,Health,OldHealth);
}

void USamAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, Health, COND_None, REPNOTIFY_Always);
}
