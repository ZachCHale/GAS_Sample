// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SamPlayerState.generated.h"

class ASamCharacterPlayer;
class UAttributeSet;

UCLASS()
class GAS_SAMPLE_API ASamPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ASamPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	UAttributeSet* GetAttributeSet() const;

	static TArray<TObjectPtr<ACharacter>> GetAllPlayerCharacters();
	static TArray<FVector> GetAllPlayerCharacterLocations();
	

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	static inline TArray<TObjectPtr<ASamPlayerState>> PlayerStateList;

};
