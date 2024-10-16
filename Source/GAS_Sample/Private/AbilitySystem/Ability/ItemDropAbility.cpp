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
	
	for (auto Drop : ItemGroup.Items)
	{
		int32 Amount = FMath::RandRange(Drop.MinimumAmount, Drop.MaximumAmount);
		for(int32 i = 0; i < Amount; i++)
		{
			AActor* InstancedItem = GetWorld()->SpawnActor<AActor>(Drop.DropObjectClass, SpawnLocation, SpawnRotation);
			UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(InstancedItem->GetRootComponent());
			if(PrimitiveComponent == nullptr)
				return;
			
			PrimitiveComponent->AddImpulse(FVector(FMath::RandRange(-1.f, 1.f),FMath::RandRange(-1.f, 1.f), FMath::RandRange(0.5f, 1.f)).GetSafeNormal() * 300.f, NAME_None, true);
		}
	}
}
