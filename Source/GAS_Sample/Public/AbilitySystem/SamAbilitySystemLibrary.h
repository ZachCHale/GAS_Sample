// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SamAbilitySystemLibrary.generated.h"

struct FCharacterClassDefaultInfo;
class ITeamInterface;
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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FCharacterClassDefaultInfo GetDefaultInfoForCharacterClass(const UObject* WorldContextObject, const ECharacterClass CharacterClass);

	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC, int32 Level = 1);

	static void InitializeDefaultAbilities(const UObject* WorldContextObject, ECharacterClass CharacterClass, UAbilitySystemComponent* ASC, int32 Level = 1);

	UFUNCTION(BlueprintCallable)
	static bool CreateAndApplyGameplayEffectToSelf(AActor* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level = 1);
	
	UFUNCTION(BlueprintCallable)
	static bool CreateAndApplyGameplayEffectToTarget(AActor* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject, int32 Level = 1 );

	UFUNCTION(BlueprintCallable)
	static FGameplayEffectSpecHandle CreateGameplayEffectSpecHandle(TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject, int32 Level = 1 );

	UFUNCTION(BlueprintCallable)
	static bool ApplyGameplayEffectSpecToTarget(AActor* Target, FGameplayEffectSpecHandle SpecHandle);

	UFUNCTION(BlueprintCallable)
	UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static TArray<FVector> GetCurrentPlayerCharacterLocations(const UObject* WorldContextObject);
	
	
};
