// Copyright ZH_Dev


#include "AbilitySystem/Ability/ContactAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

void UContactAbility::ApplyToActorsInContactRange(const FGameplayAbilityActivationInfo& ActivationInfo)
{
	bool bIsServer =  HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	AActor* Attacker = GetAvatarActorFromActorInfo();
	ETeam AllyTeam = CastChecked<ITeamInterface>(Attacker)->GetTeam();

	//Collision Channels to check
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	//Actors to ignore
	TArray<AActor*> IgnoredActors;

	TArray<AActor*> OverlappedActors;
	
	UKismetSystemLibrary::SphereOverlapActors(this, Attacker->GetActorLocation(), ContactRadius, ObjectTypes, NULL, IgnoredActors, OverlappedActors);

	for (auto Defender : OverlappedActors)
	{
		if(!Defender->Implements<UTeamInterface>())
		{
			continue;
		}
		ETeam DefenderTeam = CastChecked<ITeamInterface>(Defender)->GetTeam();
		if(AllyTeam == DefenderTeam)
			continue;

		USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToTarget(Defender, GameplayEffectClass, GetOwningActorFromActorInfo(), Attacker, Attacker);
		
	}
	if(bDrawDebugSphere)
		DrawDebugSphere(GetWorld(),Attacker->GetActorLocation(),ContactRadius, 12, FColor::Emerald,false, 3.f);
}

void UContactAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}

