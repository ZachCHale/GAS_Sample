// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Character/SamCharacterBase.h"
#include "Player/PlayerInterface.h"
#include "SamCharacterPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterPlayer : public ASamCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

	ASamCharacterPlayer();

	virtual void InitAbilityActorInfo() override;

	virtual void OnRep_PlayerState() override;

	virtual void PossessedBy(AController* NewController) override;
	
public:
	void SetCameraDistanceAhead(float Distance);

	virtual int32 GetLevel() override;
	virtual int32 GetTotalExp() override;
	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;

	bool InitHUD();
	

private:
	
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
