// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SamCharacterBase.generated.h"

UCLASS(Abstract)
class GAS_SAMPLE_API ASamCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASamCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
