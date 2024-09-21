// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SamAssetManager.generated.h"

UCLASS()
class GAS_SAMPLE_API USamAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static USamAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
	
};
