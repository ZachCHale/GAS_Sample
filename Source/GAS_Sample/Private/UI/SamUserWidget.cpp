// Copyright ZH_Dev


#include "UI/SamUserWidget.h"

void USamUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
