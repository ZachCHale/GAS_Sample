// Copyright ZH_Dev


#include "Actor/GameplayEffectActor.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
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
	
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->SetTeam(Neutral);
	
}

UAbilitySystemComponent* AGameplayEffectActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AGameplayEffectActor::OnOverlap(AActor* OtherActor)
{
	bool EffectApplied = false;
	for (auto EffectClass : GameplayEffectClasses)
	{
		EffectApplied = USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToTarget(OtherActor, EffectClass, this, this, this);
	}
}

void AGameplayEffectActor::OnEndOverlap(AActor* OtherActor)
{
}

void AGameplayEffectActor::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
