// Copyright ZH_Dev

#include "AbilitySystem/SamAttributeSet.h"
#include "SamGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "SamLogChannels.h"
#include "Character/SamCharacterBase.h"
#include "Player/ExpLevelInterface.h"

USamAttributeSet::USamAttributeSet()
{
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Primary_MovementSpeed, GetMovementSpeedAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Primary_Block, GetBlockAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Attack_Damage, GetDamageScaleAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Attack_AttackSpeed, GetAttackSpeedAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Resistance_Magic, GetMagicResistanceAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void USamAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,MovementSpeed,OldMovementSpeed);
}

void USamAttributeSet::OnRep_Block(const FGameplayAttributeData& OldBlock) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,Block,OldBlock);
}

void USamAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,MaxHealth,OldMaxHealth);
}

void USamAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,Health,OldHealth);
}

void USamAttributeSet::OnRep_DamageScale(const FGameplayAttributeData& OldDamageScale) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet,DamageScale,OldDamageScale);
}

void USamAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet, AttackSpeed, OldAttackSpeed);
}

void USamAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void USamAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USamAttributeSet, MagicResistance, OldMagicResistance);
}

void USamAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, Block, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, DamageScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USamAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);

}

void USamAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
		PostExecuteIncomingDamage(Data);
	else if(Data.EvaluatedData.Attribute == GetIncomingExpAttribute())
		PostExecuteIncomingExp(Data);
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
				SamCharacter->Auth_Die();
			}
		}
	}
}

void USamAttributeSet::PostExecuteIncomingExp(const FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingExp = GetIncomingExp();
	SetIncomingExp(0);

	AActor* TargetActor = Data.Target.GetAvatarActor();
	if(!TargetActor->Implements<UExpLevelInterface>())
		return;

	IExpLevelInterface* Player = Cast<IExpLevelInterface>(TargetActor);
	
	int32 CurrentLevel = Player->GetLevel();
	int32 CurrentExp = Player->GetTotalExp();
	int32 NewExp = CurrentExp + LocalIncomingExp;
	int NewLevel = Player->FindLevelForExp(NewExp);
	int32 LevelChange = NewLevel-CurrentLevel;
	Player->Auth_AddToExp(LocalIncomingExp);
	if(LevelChange > 0)
	{
		//LevelUp
		Player->Auth_AddToLevel(LevelChange);
	}
}
