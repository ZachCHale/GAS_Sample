// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SamGameModeBase.generated.h"

class ULevelSpawnPatternInfo;
class UCharacterClassDatabase;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterClassDatabase> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelSpawnPatternInfo> LevelSpawnPatternInfo;
	
};
