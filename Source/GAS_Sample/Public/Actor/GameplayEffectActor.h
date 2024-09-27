// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectActor.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS()
class GAS_SAMPLE_API AGameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayEffectActor();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClasses;

private:
	UFUNCTION(BlueprintCallable)
	virtual void OnOverlap(AActor* OtherActor);
	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(AActor* OtherActor);
	
};
