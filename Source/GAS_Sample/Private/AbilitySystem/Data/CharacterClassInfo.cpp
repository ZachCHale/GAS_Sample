// Copyright ZH_Dev


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return ClassDefaultInfo.FindChecked(CharacterClass);
}
