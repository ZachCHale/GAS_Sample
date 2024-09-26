// Copyright ZH_Dev

#include "AbilitySystem/SamAttributeSet.h"

#include "SamGameplayTags.h"
#include "Net/UnrealNetwork.h"

USamAttributeSet::USamAttributeSet()
{
	InitMovementSpeed(600.f);

	const FSamGameplayTags& Tags = FSamGameplayTags::Get();

	TagsToAttributes.Add(Tags.Attribute_Primary_MovementSpeed, GetMovementSpeedAttribute);
}

void USamAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,MovementSpeed,OldMovementSpeed);
}

void USamAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}
