// Copyright ZH_Dev

#include "Actor/Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "SamLogChannels.h"
#include "Actor/Interface/TeamInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(SphereComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);

	SetLifeSpan(ProjectileLifetime);
	
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;
	ProjectileMovement->Velocity = ProjectileSpeed * GetActorRotation().Vector();

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	for (auto Channel : OverlapCollisionChannels)
	{
		SphereComponent->SetCollisionResponseToChannel(Channel.GetValue(), ECR_Overlap);
	}
	
}

void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(!OtherActor->Implements<UTeamInterface>())
	{
		if(HasAuthority())
			Destroy();
		return;
	}
		

	UObject* Attacker = DamageEffectSpecHandle.Data.Get()->GetContext().GetInstigator();
	UObject* Defender = OtherActor;
	
	ETeam AllyTeam = CastChecked<ITeamInterface>(Attacker)->GetTeam();
	ETeam DefenderTeam = CastChecked<ITeamInterface>(Defender)->GetTeam();

	if(AllyTeam == DefenderTeam)
		return;
	
	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
}
