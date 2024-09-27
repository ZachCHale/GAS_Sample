// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	DefaultPlayerClass,
	DefaultEnemyClass,
};

USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DefaultAttributes;
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
};
