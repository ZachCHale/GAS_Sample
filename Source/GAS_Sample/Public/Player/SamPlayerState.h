// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PlayerInterface.h"

#include "SamPlayerState.generated.h"

class ULevelUpInfo;
class ASamCharacterPlayer;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32)

UCLASS()
class GAS_SAMPLE_API ASamPlayerState : public APlayerState, public IAbilitySystemInterface, public IPlayerInterface
{
	GENERATED_BODY()
public:
	ASamPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	UAttributeSet* GetAttributeSet() const;

	static TArray<TObjectPtr<ACharacter>> GetAllPlayerCharacters();
	static TArray<FVector> GetAllPlayerCharacterLocations();

	virtual int32 GetLevel() override { return Level; }
	virtual int32 GetTotalExp() override { return TotalExp; };

	virtual void AddToExp(int32 AddedExp) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForExp(int32 ExpValue) override;
  
	FOnPlayerStatChangedSignature ExpChangedDelegate;  
	FOnPlayerStatChangedSignature LevelChangedDelegate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	static inline TArray<TObjectPtr<ASamPlayerState>> PlayerStateList;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)  
	int32 Level = 1;  
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TotalExp)  
	int32 TotalExp = 0;  
  
	UFUNCTION()  
	void OnRep_Level(const int32& OldLevel) const;  
  
	UFUNCTION()  
	void OnRep_TotalExp(const int32& OldTotalExp) const;

};
