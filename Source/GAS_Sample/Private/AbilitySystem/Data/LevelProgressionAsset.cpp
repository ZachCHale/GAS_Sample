// Copyright ZH_Dev


#include "AbilitySystem/Data/LevelProgressionAsset.h"

int32 ULevelProgressionAsset::FindLevelFromTotalExp(int32 TotalExp)  
{  
	int32 TotalExpCounter = 0;
	int32 TotalLevelCounter = 0;
	while(TotalExp > TotalExpCounter)  
	{
		TotalExpCounter+=ExpRequirementPerLevel.AsInteger(TotalLevelCounter);  
		TotalLevelCounter++;
	}
	return TotalLevelCounter;
}

FExpProgressDetails ULevelProgressionAsset::GetExpProgressDetails(int32 TotalExp)
{
	int32 TotalLevelCounter = 0;
	int32 RemainingExpForLevelUp = TotalExp;
	FExpProgressDetails CurrentDetails(0,0,0.f);
	while(RemainingExpForLevelUp > ExpRequirementPerLevel.AsInteger(TotalLevelCounter))  
	{
		RemainingExpForLevelUp -= ExpRequirementPerLevel.AsInteger(TotalLevelCounter);
		TotalLevelCounter++;
	}
	TotalLevelCounter--;

	CurrentDetails.CurrentExp = RemainingExpForLevelUp;
	CurrentDetails.NeededExp = ExpRequirementPerLevel.AsInteger(TotalLevelCounter);
	CurrentDetails.ProgressPercentage = 1.f;
	if(CurrentDetails.NeededExp != 0)
		CurrentDetails.ProgressPercentage = static_cast<float>(RemainingExpForLevelUp)/static_cast<float>(ExpRequirementPerLevel.AsInteger(TotalLevelCounter));
	
	return CurrentDetails;
}
