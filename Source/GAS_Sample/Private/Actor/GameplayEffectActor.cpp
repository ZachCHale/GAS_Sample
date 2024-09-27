// Copyright ZH_Dev


#include "Actor/GameplayEffectActor.h"

#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
AGameplayEffectActor::AGameplayEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(MeshComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	SphereComponent->SetupAttachment(GetRootComponent());
}

void AGameplayEffectActor::OnOverlap(AActor* OtherActor)
{
	bool EffectApplied = false;
	for (auto EffectClass : GameplayEffectClasses)
	{
		EffectApplied = USamAbilitySystemLibrary::ApplyGameplayEffectToTarget(OtherActor, EffectClass, this);
	}
	if(EffectApplied)
	{
		
	}
}

void AGameplayEffectActor::OnEndOverlap(AActor* OtherActor)
{
}
