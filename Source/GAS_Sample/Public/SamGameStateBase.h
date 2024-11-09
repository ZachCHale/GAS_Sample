// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInterface.h"
#include "SamGameStateBase.generated.h"

class ULevelUpInfo;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStatChangedSignature, int32)

UCLASS()
class GAS_SAMPLE_API ASamGameStateBase : public AGameStateBase , public IPlayerInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	FOnGameStatChangedSignature ExpChangedDelegate;  
	FOnGameStatChangedSignature LevelChangedDelegate;

	virtual int32 GetLevel() override { return SharedPlayerLevel; }
	virtual int32 GetTotalExp() override { return SharedPlayerExp; };

	virtual void PostInitializeComponents() override;

	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForExp(int32 ExpValue) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	TArray<ACharacter*> GetAllPlayerCharacters();

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerLevel)  
	int32 SharedPlayerLevel = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SharedPlayerExp)  
	int32 SharedPlayerExp = 0;

	UFUNCTION()  
	void OnRep_SharedPlayerLevel(int32 OldLevel) const;  
  
	UFUNCTION()  
	void OnRep_SharedPlayerExp(int32 OldExp) const;
	
	void PauseAndOpenLevelUpScreen();
	
	
};
