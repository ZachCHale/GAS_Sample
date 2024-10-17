// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	DefaultPlayerClass,
	DefaultEnemyClass,
};

USTRUCT(BlueprintType)
struct FExpRewardInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 OrbCount = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 OrbExpValue = 1;
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Kill Reward")
	FExpRewardInfo ExpReward;
	
};

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterClass,FCharacterClassDefaultInfo> ClassDefaultInfo;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
