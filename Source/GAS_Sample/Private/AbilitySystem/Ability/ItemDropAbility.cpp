// Copyright ZH_Dev


#include "AbilitySystem/Ability/ItemDropAbility.h"

#include "SamLogChannels.h"

void UItemDropAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	for (auto DropTable : DropTables)
	{
		int32 TotalWeight = 0;
		for (auto DropGroup : DropTable.PossibleItems)
		{
			TotalWeight += DropGroup.DropWeight;
		}
		int32 Roll = FMath::RandRange(0, TotalWeight);
		for (auto DropGroup : DropTable.PossibleItems)
		{
			Roll -= DropGroup.DropWeight;
			if(Roll <= 0)
			{
				DropItems(DropGroup);
				break;
			}
		}
	}
}

void UItemDropAbility::DropItems(const FItemDropGroup& ItemGroup)
{
	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FRotator SpawnRotation = FRotator();

	UE_LOG(SamLog, Log, TEXT("Dropping Items"));
	for (auto Drop : ItemGroup.Items)
	{
		int32 Amount = FMath::RandRange(Drop.MinimumAmount, Drop.MaximumAmount);
		for(int32 i = 0; i < Amount; i++)
		{
			AActor* InstancedItem = GetWorld()->SpawnActor<AActor>(Drop.DropObjectClass, SpawnLocation, SpawnRotation);
			UStaticMeshComponent* MeshComponent = InstancedItem->GetComponentByClass<UStaticMeshComponent>();
			if(MeshComponent != nullptr)
			MeshComponent->AddForce(FVector(FMath::RandRange(-1.f, 1.f),FMath::RandRange(-1.f, 1.f), FMath::RandRange(0.5f, 1.f)).GetSafeNormal() * 3000.f);
		}
	}
}
