// Copyright ZH_Dev

#include "SamGameplayTags.h"  

#include "GameplayTagsManager.h"

FSamGameplayTags FSamGameplayTags::GameplayTags;  
  
void FSamGameplayTags::InitializeNativeGameplayTags()  
{  
	GameplayTags.Attribute_Primary_MovementSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.MovementSpeed"),FString("Character Movement Speed"));

	GameplayTags.Attribute_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.MaxHealth"),FString("Character Max Health"));

	GameplayTags.Attribute_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Vital.Health"),FString("Character Current Health"));

	GameplayTags.InputBinding_SlottedAbility_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputBinding.SlottedAbility.1"),FString("Input activates the first slotted ability"));
}