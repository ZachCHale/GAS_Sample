// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SamGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API USamGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	
};
