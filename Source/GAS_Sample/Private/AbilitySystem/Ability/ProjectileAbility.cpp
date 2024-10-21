// Copyright ZH_Dev


#include "AbilitySystem/Ability/ProjectileAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SamGameplayTags.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
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

	FGameplayEffectSpecHandle SpecHandle = USamAbilitySystemLibrary::CreateGameplayEffectSpecHandle(DamageEffectClass, OwningActor, Projectile, AvatarActor);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, SamTags::CallerMagnitudeTags::CallerMagnitude_IncomingDamage, DamageValue);
	
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
