// Copyright ZH_Dev


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelFromTotalExp(int32 TotalExp)  
{  
	int32 TotalExpCounter = 0;  
	for(int i = 0; i < LevelUpInfo.Num(); i++)  
	{
		TotalExpCounter+=LevelUpInfo[i].ExpRequirement;  
		if(TotalExp < TotalExpCounter)  
			return i+1;  
	}
	return LevelUpInfo.Num();
}

FExpProgressDetails ULevelUpInfo::GetExpProgressDetails(int32 TotalExp)
{
	int32 TotalExpCounter = 0;
	int32 RemainingExpForLevelUp = TotalExp;
	FExpProgressDetails CurrentDetails(0,0,0.f);
	for(int i = 0; i < LevelUpInfo.Num(); i++)
	{
		TotalExpCounter+=LevelUpInfo[i].ExpRequirement;
		if(TotalExp < TotalExpCounter)
		{
			CurrentDetails.CurrentExp = RemainingExpForLevelUp;
			CurrentDetails.NeededExp = LevelUpInfo[i].ExpRequirement;
			CurrentDetails.ProgressPercentage = 1.f;
			if(CurrentDetails.NeededExp != 0)
				CurrentDetails.ProgressPercentage = static_cast<float>(RemainingExpForLevelUp)/static_cast<float>(LevelUpInfo[i].ExpRequirement);
			break;
		}
		RemainingExpForLevelUp -= LevelUpInfo[i].ExpRequirement;
	}
	
	return CurrentDetails;
}
