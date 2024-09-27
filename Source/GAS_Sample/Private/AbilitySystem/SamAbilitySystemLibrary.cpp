// Copyright ZH_Dev


#include "AbilitySystem/SamAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "SamGameModeBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"

TObjectPtr<UCharacterClassInfo> USamAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	ASamGameModeBase* SamGameMode = Cast<ASamGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(SamGameMode == nullptr) return nullptr;
	return SamGameMode->CharacterClassInfo;
}

void USamAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(ClassInfo == nullptr) return;
	FCharacterClassDefaultInfo DefaultInfo = ClassInfo->ClassDefaultInfo[CharacterClass];

	AActor* AvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle DefaultAttributesContextHandle = ASC->MakeEffectContext();
	DefaultAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle DefaultAttributesSpecHandle = ASC->MakeOutgoingSpec(DefaultInfo.DefaultAttributes, Level, DefaultAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*DefaultAttributesSpecHandle.Data.Get());
}

bool USamAbilitySystemLibrary::ApplyGameplayEffectToTarget(AActor* Target,
	TSubclassOf<UGameplayEffect> GameplayEffectClass, UObject* SourceObject, int32 Level)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if(TargetASC == nullptr)
		return false;
	check(GameplayEffectClass);
	check(SourceObject);
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject);
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}
