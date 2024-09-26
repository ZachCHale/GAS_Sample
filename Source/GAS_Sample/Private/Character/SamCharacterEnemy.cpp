// Copyright ZH_Dev


#include "Character/SamCharacterEnemy.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "Player/SamPlayerState.h"

ASamCharacterEnemy::ASamCharacterEnemy()
{
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");
}

void ASamCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	BindToAttributeChanges();
}

void ASamCharacterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	MoveTowardsClosestPlayer();
}

void ASamCharacterEnemy::MoveTowardsClosestPlayer()
{
	TArray<FVector> PlayerLocations = ASamPlayerState::GetAllPlayerCharacterLocations();
	if(PlayerLocations.Num() == 0) return;
	FVector TargetPosition = PlayerLocations[0];
	float TargetDistance = FVector::Dist(GetActorLocation(), TargetPosition);
	for (auto PlayerLocation : PlayerLocations)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerLocation);
		if(Distance < TargetDistance)
		{
			TargetDistance = Distance;
			TargetPosition = PlayerLocation;
		}
	}
	FVector Direction = ((TargetPosition - GetActorLocation()) * FVector(1.f,1.f,0.f)).GetSafeNormal();
	FaceRotation(Direction.Rotation());
	AddMovementInput(Direction);
}

void ASamCharacterEnemy::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
