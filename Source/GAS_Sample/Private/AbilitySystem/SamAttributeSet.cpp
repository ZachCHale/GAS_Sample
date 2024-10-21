// Copyright ZH_Dev

#include "AbilitySystem/SamAttributeSet.h"
#include "SamGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "SamLogChannels.h"
#include "Character/SamCharacterBase.h"
#include "Player/PlayerInterface.h"

USamAttributeSet::USamAttributeSet()
{
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Primary_MovementSpeed, GetMovementSpeedAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(SamTags::AttributeTags::Attribute_Vital_Health, GetHealthAttribute);
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
				SamCharacter->Die();
			}
		}
	}
}

void USamAttributeSet::PostExecuteIncomingExp(const FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingExp = GetIncomingExp();
	SetIncomingExp(0);

	AActor* TargetActor = Data.Target.GetAvatarActor();
	if(!TargetActor->Implements<UPlayerInterface>())
		return;

	IPlayerInterface* Player = Cast<IPlayerInterface>(TargetActor);
	
	int32 CurrentLevel = Player->GetLevel();
	int32 CurrentExp = Player->GetTotalExp();
	int32 NewExp = CurrentExp + LocalIncomingExp;
	int NewLevel = Player->FindLevelForExp(NewExp);
	int32 LevelChange = NewLevel-CurrentLevel;
	if(LevelChange > 0)
	{
		//LevelUp
		Player->AddToLevel(LevelChange);
		UE_LOG(SamLog, Log, TEXT("Level Up %d"), Player->GetLevel());
	}
	Player->AddToExp(LocalIncomingExp);
}
