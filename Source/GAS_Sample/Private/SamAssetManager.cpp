// Copyright ZH_Dev

#include "SamAssetManager.h"

#include "SamGameplayTags.h"
#include "SamLogChannels.h"

USamAssetManager& USamAssetManager::Get()
{
	check(GEngine);
	USamAssetManager* SamAssetManager = Cast<USamAssetManager>(GEngine->AssetManager);
	return *SamAssetManager;
}

void USamAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FSamGameplayTags::Get().InitializeNativeGameplayTags();
}
