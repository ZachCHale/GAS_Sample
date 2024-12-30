#pragma once
#include "SamWidgetControllerEvents.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature_Dynamic, float, NewValue);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntStatChangedSignature_Dynamic, int, NewValue);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProgressBarStatChangedSignature_Dynamic, int, CurrentValue, int, MaxValue, float, ProgressPercentage);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUpSelectionSignature_Dynamic);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCharacterDeathSignature_Dynamic);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCharacterReviveSignature_Dynamic);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloatStatChangedSignature_Dynamic, float, NewValue);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyCountChangedSignature_Dynamic, int, ReadyPlayerCount, int, TotalPlayerCount);

