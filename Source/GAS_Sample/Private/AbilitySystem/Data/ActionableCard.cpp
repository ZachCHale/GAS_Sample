// Copyright ZH_Dev


#include "AbilitySystem/Data/ActionableCard.h"

#include "SamLogChannels.h"

void UActionableCard::ExecuteCardAction_Implementation(APlayerState* TargetPlayerState, FGameplayTag ActionCardTag)
{
	UE_LOG(SamLog, Warning, TEXT("No Action Specified. Override ExecuteCardAction."))
}
