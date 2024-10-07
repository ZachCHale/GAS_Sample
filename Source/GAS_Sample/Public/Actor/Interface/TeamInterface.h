// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TeamInterface.generated.h"

UENUM(BlueprintType)
enum ETeam : uint32
{
	//Allied To Player
	Player,
	//Hostile To Player
	Enemy,
	//Usually a valid target for players but invalid target for enemies.
	Neutral
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTeamInterface : public UInterface
{
	GENERATED_BODY()
};

class GAS_SAMPLE_API ITeamInterface
{
	GENERATED_BODY()

public:
	virtual ETeam GetTeam();
	
	static bool IsRelativeEnemy(TScriptInterface<ITeamInterface> Source, TScriptInterface<ITeamInterface> Target);
};
