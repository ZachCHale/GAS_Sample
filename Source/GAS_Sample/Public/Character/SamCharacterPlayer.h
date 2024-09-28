// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Character/SamCharacterBase.h"
#include "SamCharacterPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterPlayer : public ASamCharacterBase
{
	GENERATED_BODY()

	ASamCharacterPlayer();

	virtual void InitAbilityActorInfo() override;

	virtual void OnRep_PlayerState() override;

	virtual void PossessedBy(AController* NewController) override;

	
public:
	void SetCameraDistanceAhead(float Distance);
	

private:
	
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
