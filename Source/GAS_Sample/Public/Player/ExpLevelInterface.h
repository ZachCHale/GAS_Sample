// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExpLevelInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExpLevelInterface : public UInterface
{
	GENERATED_BODY()
};

class GAS_SAMPLE_API IExpLevelInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetTotalExp() = 0;
	virtual int32 GetLevel() = 0;
	virtual void Auth_AddToExp(int32 AddedExp) = 0;
	virtual void Auth_AddToLevel(int32 AddedLevels) = 0;
	virtual int32 FindLevelForExp(int32 ExpValue) = 0;
};
