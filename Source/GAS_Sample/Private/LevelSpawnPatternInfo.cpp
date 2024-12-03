// Copyright ZH_Dev


#include "LevelSpawnPatternInfo.h"

FSpawnPatternInfo ULevelSpawnPatternInfo::GetSpawnPatternInfoFromTag(FGameplayTag LevelTag)
{
	return LevelSpawnPatternInfo.FindChecked(LevelTag);
}
