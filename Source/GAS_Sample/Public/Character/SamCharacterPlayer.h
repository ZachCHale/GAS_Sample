// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "Character/SamCharacterBase.h"
#include "Player/ExpLevelInterface.h"
#include "SamCharacterPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReviveSignature_Dynamic, ASamCharacterBase*, CharacterInstance);

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterPlayer : public ASamCharacterBase, public IExpLevelInterface
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
	virtual void Auth_AddToExp(int32 AddedExp) override;
	virtual void Auth_AddToLevel(int32 AddedLevels) override;
	virtual int32 FindLevelForExp(int32 ExpValue) override;

	virtual void Auth_Die() override;

	void Auth_Revive();

	UPROPERTY(BlueprintAssignable)
	FOnReviveSignature_Dynamic OnReviveDelegate;
	

private:
	
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	TObjectPtr<UCameraComponent> CameraComponent;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastHandleRevive();
	
};
