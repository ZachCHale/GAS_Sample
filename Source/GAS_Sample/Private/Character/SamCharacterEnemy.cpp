// Copyright ZH_Dev


#include "Character/SamCharacterEnemy.h"

#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Player/SamPlayerState.h"
#include "UI/SamUserWidget.h"

ASamCharacterEnemy::ASamCharacterEnemy()
{
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(RootComponent);
}

void ASamCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	if(USamUserWidget* SamUserWidget = Cast<USamUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		SamUserWidget->SetWidgetController(this);
	}
	
	const USamAttributeSet* SamAS = CastChecked<USamAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(SamAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});
	
	OnHealthChanged.Broadcast(SamAS->GetHealth());
	OnMaxHealthChanged.Broadcast(SamAS->GetMaxHealth());
}

void ASamCharacterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bIsDead) return;
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
	BindToAttributeChanges();
	InitDefaultAttributes();
	InitDefaultAbilities();
}


