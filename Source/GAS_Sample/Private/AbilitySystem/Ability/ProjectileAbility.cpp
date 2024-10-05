// Copyright ZH_Dev


#include "AbilitySystem/Ability/ProjectileAbility.h"

#include "AbilitySystemComponent.h"
#include "Actor/Projectile.h"

void UProjectileAbility::SpawnProjectile(const FTransform& ProjectileSpawnTransform, const FGameplayAbilityActivationInfo& ActivationInfo)
{
	//Handled by Server Only
	bool bIsServer = HasAuthority(&ActivationInfo);
	if(!bIsServer) return;
	check(ProjectileClass);
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, ProjectileSpawnTransform, OwningActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddInstigator(AvatarActor, Projectile);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
	
	Projectile->DamageEffectSpecHandle = SpecHandle;

	Projectile->FinishSpawning(ProjectileSpawnTransform);
	
}

FTransform UProjectileAbility::GetInstigatorTransform()
{
	return GetAvatarActorFromActorInfo()->GetTransform();
}

void UProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}
