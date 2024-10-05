// Copyright ZH_Dev

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class GAS_SAMPLE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<enum ECollisionChannel>> OverlapCollisionChannels;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditDefaultsOnly)
	float ProjectileSpeed;
	UPROPERTY(EditDefaultsOnly)
	float ProjectileGravityScale;
	UPROPERTY(EditDefaultsOnly)
	float ProjectileLifetime;
	
};
