// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "SamWidgetControllerEvents.h"
#include "Character/SamCharacterBase.h"
#include "SamCharacterEnemy.generated.h"

class UWidgetComponent;

/**
 * 
 */
UCLASS()
class GAS_SAMPLE_API ASamCharacterEnemy : public ASamCharacterBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnProgressBarStatChangedSignature_Dynamic OnHealthChanged;
	
private:
	ASamCharacterEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void Auth_MoveTowardsClosestPlayer();

protected:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
