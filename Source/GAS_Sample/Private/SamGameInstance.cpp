// Copyright ZH_Dev


#include "SamGameInstance.h"

#include "SamLogChannels.h"

void USamGameInstance::Init()
{
	Super::Init();
	GetEngine()->OnNetworkFailure().AddUObject(this, &USamGameInstance::HandleNetworkFailure);
	UE_LOG(SamLog, Log, TEXT("SamGameInstance Initialized"))
}

void USamGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	UE_LOG(SamLog, Log, TEXT("SamGameInstance Handling Network failure"))
	if(FailureType == ENetworkFailure::Type::PendingConnectionFailure)
	{
		UE_LOG(SamLog, Log, TEXT("SamGameInstance Handling Pending Connection Failure"))
	}
}
