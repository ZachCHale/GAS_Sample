// Copyright ZH_Dev


#include "SamGameStateBase.h"

#include "SamLogChannels.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Net/UnrealNetwork.h"

void ASamGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerLevel);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerExp);
}

void ASamGameStateBase::AddToExp(int32 AddedExp)
{
	if(!HasAuthority()) return;
	SharedPlayerExp += AddedExp;  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
	UE_LOG(SamLog, Log, TEXT("SharedExp %d"), SharedPlayerExp);
}

void ASamGameStateBase::AddToLevel(int32 AddedLevels)
{
	if(!HasAuthority()) return;
	SharedPlayerLevel+=AddedLevels;  
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);
	UE_LOG(SamLog, Log, TEXT("SharedLevel %d"), SharedPlayerLevel);
}

int32 ASamGameStateBase::FindLevelForExp(int32 ExpValue)
{
	check(LevelUpInfo)
	return LevelUpInfo->FindLevelFromTotalExp(ExpValue);
}

void ASamGameStateBase::OnRep_SharedPlayerLevel(int32 OldLevel) const
{  
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp(int32 OldTotalExp) const
{  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);  
}