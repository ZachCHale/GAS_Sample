// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class GAS_SAMPLE_API IPlayerInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetTotalExp() = 0;
	virtual int32 GetLevel() = 0;
	virtual void AddToExp(int32 AddedExp) = 0;
	virtual void AddToLevel(int32 AddedLevels) = 0;
	virtual int32 FindLevelForExp(int32 ExpValue) = 0;
};
