// Copyright ZH_Dev


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfoFromTag(FGameplayTag CharacterClassTag)
{
	return ClassDefaultInfo.FindChecked(CharacterClassTag);
}
