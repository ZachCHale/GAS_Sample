// Copyright ZH_Dev

#include "AbilitySystem/SamAttributeSet.h"

#include "SamGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Character/SamCharacterBase.h"

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

void USamAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
		PostExecuteIncomingDamage(Data);
}

void USamAttributeSet::PostExecuteIncomingDamage(const FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0);
	if(LocalIncomingDamage > 0)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		if(const bool bFatal = NewHealth <= 0.f)
		{
			ASamCharacterBase* SamCharacter = Cast<ASamCharacterBase>(Data.Target.GetAvatarActor());
			if(SamCharacter != nullptr)
			{
				SamCharacter->Die();
			}
		}
	}
}
