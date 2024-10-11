// Copyright ZH_Dev


#include "AbilitySystem/Data/LevelUpInfo.h"


int32 ULevelUpInfo::FindLevelFromTotalExp(int32 TotalExp)  
{  
	int32 TotalExpCounter = 0;  
	for(int i = 0; i < LevelUpInfo.Num(); i++)  
	{       TotalExpCounter+=LevelUpInfo[i].ExpRequirement;  
		if(TotalExp < TotalExpCounter)  
			return i+1;  
	}
	return LevelUpInfo.Num();
}

float ULevelUpInfo::GetPercentProgress(int32 TotalExp)
{
	int32 TotalExpCounter = 0;
	for(int i = 0; i < LevelUpInfo.Num(); i++)
	{
		TotalExpCounter+=LevelUpInfo[i].ExpRequirement;
		if(TotalExp < TotalExpCounter)
		{
			int32 ExpThisLevel = (TotalExp - TotalExpCounter) + LevelUpInfo[i].ExpRequirement;
			float Percent = static_cast<float>(ExpThisLevel)/static_cast<float>(LevelUpInfo[i].ExpRequirement);
			return Percent*100;
		}
	}
	return 100.f;
}

FExperienceProgressDetails ULevelUpInfo::GetExperienceProgressDetails(int32 TotalXP)
{
	int32 TotalExpCounter = 0;
	int32 RemainingXP = TotalXP;
	FExperienceProgressDetails CurrentDetails(0,0,0.f);
	for(int i = 0; i < LevelUpInfo.Num(); i++)
	{
		TotalExpCounter+=LevelUpInfo[i].ExpRequirement;
		if(TotalXP < TotalExpCounter)
		{
			CurrentDetails.CurrentExperience = RemainingXP;
			CurrentDetails.NeededExperience = LevelUpInfo[i].ExpRequirement;
			CurrentDetails.ProgressPercentage = 1.f;
			if(CurrentDetails.NeededExperience != 0)
				CurrentDetails.ProgressPercentage = static_cast<float>(RemainingXP)/static_cast<float>(LevelUpInfo[i].ExpRequirement);
			break;
		}
		RemainingXP -= LevelUpInfo[i].ExpRequirement;
	}
	
	return CurrentDetails;
}
