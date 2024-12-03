// Copyright ZH_Dev


#include "SubSystem/EnemySpawnManagemenSubsystem.h"

#include "LevelSpawnPatternInfo.h"
#include "SamGameplayTags.h"
#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "Character/SamCharacterBase.h"

void UEnemySpawnManagemenSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	TimeTillNextSpawnAttempt = DelayFromGameStart;
	ScopedWorld = InWorld;
	
}

void UEnemySpawnManagemenSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeTillNextSpawnAttempt -= DeltaTime;
	if(TimeTillNextSpawnAttempt <= 0)
	{
		StartSpawnAttempt();
		TimeTillNextSpawnAttempt = SpawnAttemptFreq;
	}
}

bool UEnemySpawnManagemenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if(!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	ENetMode netmode = World->GetNetMode();
	return netmode  < NM_Client;
}

bool UEnemySpawnManagemenSubsystem::IsTickable() const
{
	if(!Super::IsTickable())
	{
		return false;
	}
	return IsAllowedToTick();
}

TStatId UEnemySpawnManagemenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEnemySpawnManagemenSubsystem, STATGROUP_Tickables);
}

void UEnemySpawnManagemenSubsystem::StartSpawnAttempt()
{
	UE_LOG(SamLog, Log, TEXT("Attempt Spawn"));
	UCharacterClassInfo* ClassInfo = USamAbilitySystemLibrary::GetCharacterClassInfo(ScopedWorld);
	FCharacterClassDefaultInfo DefaultEnemyInfo = ClassInfo->GetClassDefaultInfoFromTag(SamTags::CharacterClass::CharacterClass_Enemy_Default);

	ULevelSpawnPatternInfo* SpawnPatternInfoDA = USamAbilitySystemLibrary::GetLevelSpawnPatternInfo(ScopedWorld);
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(ScopedWorld);

	float GameProgress = SamGS->Auth_GetCurrentGameProgress();
	FGameplayTag LevelTag = SamTags::Levels::Level_Gameplay_Default;

	FSpawnPatternInfo LevelInfo = SpawnPatternInfoDA->GetSpawnPatternInfoFromTag(LevelTag);

	TArray<FVector> PlayerPositions = USamAbilitySystemLibrary::GetCurrentPlayerCharacterLocations(ScopedWorld);
	int32 PlayerCount = PlayerPositions.Num();
	
	if(PlayerCount == 0)
		return;

	for (FEnemySpawnPatternInfo SpawnPattern : LevelInfo.EnemySpawnPatterns)
	{
		int32 UnitCap = SpawnPattern.GetUnitCapAtTime(GameProgress);
		//Check how many of this unit are currently alive
		int32 UnitCount = 0;
		if(LiveUnitCount.Contains(SpawnPattern.CharacterClassTag))
			UnitCount = LiveUnitCount[SpawnPattern.CharacterClassTag];

		//Spawn units up to the current unit cap
		int32 UnitsToSpawn = UnitCap-UnitCount;
		for(int32 i = 0; i < UnitsToSpawn; i++)
		{
			//Choose a random player to spawn around
			int32 RandomPlayerIndex = FMath::RandRange(0, PlayerCount-1);
			//Spawn Unit
			const FVector SpawnPosition = PlayerPositions[RandomPlayerIndex] + (FVector(FMath::RandRange(-1.f, 1.f),FMath::RandRange(-1.f, 1.f), 0.f).GetSafeNormal() * SpawnDistance) + FVector(0.f,0.f,VerticalOffset);
			const FRotator SpawnRotation = FRotator();
			ASamCharacterBase* NewUnit = ScopedWorld->SpawnActor<ASamCharacterBase>(DefaultEnemyInfo.CharacterClass, SpawnPosition, SpawnRotation);
			if(NewUnit == nullptr)
				continue;
			//Assert that tags provided by the SpawnPattern and the Character Class itself are the same.
			check(NewUnit->GetCharacterClassTag().MatchesTagExact(SpawnPattern.CharacterClassTag))
			//Bind to unit death to decrement unit count
			NewUnit->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleUnitDeath);
			//Increment Unit Count
			if(LiveUnitCount.Contains(SpawnPattern.CharacterClassTag))
				LiveUnitCount[SpawnPattern.CharacterClassTag]++;
			else
			{
				LiveUnitCount.Add(SpawnPattern.CharacterClassTag, 1);
			}
		}
	}
	
	/*UE_LOG(SamLog, Log, TEXT("Class: %s"), *DefaultEnemyInfo.CharacterClass->GetName());

	for (auto PlayerPosition : PlayerPositions)
	{
		const FVector SpawnPosition = PlayerPosition + (FVector(FMath::RandRange(-1.f, 1.f),FMath::RandRange(-1.f, 1.f), 0.f).GetSafeNormal() * SpawnDistance) + FVector(0.f,0.f,VerticalOffset);
		const FRotator SpawnRotation = FRotator();
		ScopedWorld->SpawnActor<ASamCharacterBase>(DefaultEnemyInfo.CharacterClass, SpawnPosition, SpawnRotation);
	}*/
}

void UEnemySpawnManagemenSubsystem::HandleUnitDeath(ASamCharacterBase* UnitInstance)
{
	FGameplayTag CharacterClassTag = UnitInstance->GetCharacterClassTag();
	check(LiveUnitCount.Contains(CharacterClassTag));
	LiveUnitCount[CharacterClassTag]--;
	check(LiveUnitCount[CharacterClassTag]>=0);
}
