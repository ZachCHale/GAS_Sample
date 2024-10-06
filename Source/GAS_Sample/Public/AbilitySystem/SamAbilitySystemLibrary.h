// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SamAbilitySystemLibrary.generated.h"

struct FGameplayEffectSpecHandle;
class UOverlayWidgetController;
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

	static void InitializeDefaultAbilities(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC, int32 Level = 1);
	
	UFUNCTION(BlueprintCallable)
	static bool CreateAndApplyGameplayEffectToTarget(AActor* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject, int32 Level = 1 );

	UFUNCTION(BlueprintCallable)
	static FGameplayEffectSpecHandle CreateGameplayEffectSpecHandle(TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject, int32 Level = 1 );

	UFUNCTION(BlueprintCallable)
	static bool ApplyGameplayEffectSpecToTarget(AActor* Target, FGameplayEffectSpecHandle SpecHandle);

	UFUNCTION(BlueprintCallable)
	UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
};
