// Copyright ZH_Dev


#include "AbilitySystem/Ability/ContactAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SamGameplayTags.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Actor/Interface/TeamInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UContactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UContactAbility::Auth_ApplyToActorsInContactRange(const FGameplayAbilityActivationInfo& ActivationInfo)
{
	bool bIsServer =  HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	check(SourceActor->Implements<UTeamInterface>())

	//Collision Channels to check
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	//Actors to ignore
	TArray<AActor*> IgnoredActors;

	TArray<AActor*> OverlappedActors;
	
	UKismetSystemLibrary::SphereOverlapActors(this, SourceActor->GetActorLocation(), ContactRadius, ObjectTypes, NULL, IgnoredActors, OverlappedActors);

	for (auto OtherActor : OverlappedActors)
	{
		if(!OtherActor->Implements<UTeamInterface>())
			continue;
		if(!ITeamInterface::IsRelativeEnemy(SourceActor, OtherActor))
			continue;

		FGameplayEffectSpecHandle SpecHandle = USamAbilitySystemLibrary::CreateGameplayEffectSpecHandle(
			GameplayEffectClass, GetOwningActorFromActorInfo(), SourceActor, SourceActor);

		for (auto [DamageTypeTag, Value] : DamageTypeValues)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, Value);
		}
		
		USamAbilitySystemLibrary::ApplyGameplayEffectSpecToTarget(OtherActor, SpecHandle);
	}
	if(bDrawDebugSphere)
		DrawDebugSphere(GetWorld(),SourceActor->GetActorLocation(),ContactRadius, 12, FColor::Emerald,false, 3.f);
}

void UContactAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}

