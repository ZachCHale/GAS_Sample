// Copyright ZH_Dev


#include "AbilitySystem/Data/UpgradeInfo.h"

#include "SamLogChannels.h"

FUpgradeInfoItem UUpgradeInfo::GetUpgradeInfoFromTag(FGameplayTag UpgradeTag) const
{
	for (auto Upgrade : UpgradeInfo)
	{
		if(Upgrade.UpgradeTag.MatchesTagExact(UpgradeTag))
		{
			 return Upgrade;
		}
	}
	UE_LOG(SamLog, Error, TEXT("Could not find Upgrade info with associated gameplaytag: %s"), *UpgradeTag.GetTagName().ToString());
	return FUpgradeInfoItem();
}

TArray<FGameplayTag> UUpgradeInfo::GetRandomUpgradeTags(int32 TagCount)
{
	check(TagCount<=UpgradeInfo.Num());
	TArray<FGameplayTag> RandomTags;
	TArray<int32> RandomIndices;
	for(int i = 0; i < TagCount; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, UpgradeInfo.Num()-1-i);
		for (auto Index : RandomIndices)
		{
			if(RandomIndex>=Index)
				RandomIndex++;
		}
		RandomIndices.Add(RandomIndex);
		RandomIndices.Sort();
		RandomTags.Add(UpgradeInfo[RandomIndex].UpgradeTag);
	}
	return RandomTags;
}
