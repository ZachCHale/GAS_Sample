// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SamAbilitySystemLibrary.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static TObjectPtr<UCharacterClassInfo> GetCharacterClassInfo(const UObject* WorldContextObject);

	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC, int32 Level = 1);

	UFUNCTION(BlueprintCallable)
	static bool ApplyGameplayEffectToTarget(AActor* Target,
	TSubclassOf<UGameplayEffect> GameplayEffectClass, UObject* SourceObject, int32 Level = 1);
	
};
