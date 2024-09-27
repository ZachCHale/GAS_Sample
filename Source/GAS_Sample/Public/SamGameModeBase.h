// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SamGameModeBase.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	
	
};
