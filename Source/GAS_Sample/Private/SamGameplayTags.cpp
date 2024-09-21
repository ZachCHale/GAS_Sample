// Copyright ZH_Dev

#include "SamGameplayTags.h"  

#include "GameplayTagsManager.h"

FSamGameplayTags FSamGameplayTags::GameplayTags;  
  
void FSamGameplayTags::InitializeNativeGameplayTags()  
{  
	GameplayTags.Sample_Test_Tag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Sample.Test.Tag"),FString("For testing purposes only"));
}