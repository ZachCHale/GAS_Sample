// Copyright ZH_Dev

#include "SamGameplayTags.h"  

#include "GameplayTagsManager.h"

FSamGameplayTags FSamGameplayTags::GameplayTags;  
  
void FSamGameplayTags::InitializeNativeGameplayTags()  
{  
	GameplayTags.Attribute_Primary_MovementSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.MovementSpeed"),FString("Character Movement Speed"));
}