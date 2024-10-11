#pragma once
#include "SamWidgetControllerEvents.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntStatChangedSignature, int, NewValue);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProgressBarStatChangedSignature, int, CurrentValue, int, MaxValue, float, ProgressPercentage);

