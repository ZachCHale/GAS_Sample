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
	virtual int32 GetXP() override { return TotalExperience; };

	virtual void AddToXP(int32 AddedXP) override;
	virtual void AddToLevel(int32 AddedLevels) override;

	virtual int32 FindLevelForXP(int32 XPValue) override;
  
	FOnPlayerStatChangedSignature ExperienceChangedDelegate;  
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
  
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TotalExperience)  
	int32 TotalExperience = 0;  
  
	UFUNCTION()  
	void OnRep_Level(int32 OldLevel);  
  
	UFUNCTION()  
	void OnRep_TotalExperience(int32 OldTotalExperience);

};
