// Copyright ZH_Dev


#include "AbilitySystem/Ability/ItemDropAbility.h"

#include "SamLogChannels.h"

void UItemDropAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	check(ItemClass);
	
	int32 NumberToSpawn = bUseCustomSpawnAmountCalculation ? CustomSpawnAmountCalculation() : SpawnAmount;
	
	for(int32 i = 0; i < NumberToSpawn; i++)
	{
		FTransform SpawnTransform = InitItemTransform();
		AActor* InstancedItem = GetWorld()->SpawnActorDeferred<AActor>(ItemClass, SpawnTransform);
		InitItemReplication(InstancedItem);
		InitItemExtraData(InstancedItem);
		InstancedItem->FinishSpawning(SpawnTransform);
		AddImpulseToItem(InstancedItem);

	}
}

void UItemDropAbility::InitItemExtraData_Implementation(AActor* ItemInstance)
{
}

FTransform UItemDropAbility::InitItemTransform_Implementation()
{
	return GetAvatarActorFromActorInfo()->GetActorTransform();
}

void UItemDropAbility::InitItemReplication_Implementation(AActor* ItemInstance)
{
	ItemInstance->SetReplicates(true);
	ItemInstance->SetReplicateMovement(true);
}

void UItemDropAbility::AddImpulseToItem(const AActor* ItemInstance) const
{
	if(ThrowItemStrength == 0.f) return;
	
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ItemInstance->GetRootComponent());
	if(PrimitiveComponent == nullptr)
		return;
	PrimitiveComponent->AddImpulse(FVector(FMath::RandRange(-1.f, 1.f),FMath::RandRange(-1.f, 1.f), FMath::RandRange(0.5f, 1.f)).GetSafeNormal() * ThrowItemStrength, NAME_None, true);
}

int32 UItemDropAbility::CustomSpawnAmountCalculation_Implementation()
{
	UE_LOG(SamLog, Warning, TEXT("Item Drop Ability is set to use custom amount calculation but a blueprint implementation for CustomSpawnAmoundCalculation could not be found."));
	return 1;
}
