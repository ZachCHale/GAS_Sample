// Copyright ZH_Dev


#include "AbilitySystem/SamAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "SamGameModeBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/GameplayStatics.h"

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
