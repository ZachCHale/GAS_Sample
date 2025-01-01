// Copyright ZH_Dev


#include "AbilitySystem/Data/CharacterClassDatabase.h"

FCharacterClassDefaultInfo UCharacterClassDatabase::GetClassDefaultInfoFromTag(FGameplayTag CharacterClassTag)
{
	return ClassDefaultInfo.FindChecked(CharacterClassTag);
}
