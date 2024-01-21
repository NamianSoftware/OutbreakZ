// OutbreakZ: Mobile. All rights reserved.


#include "UI/Widgets/Components/InteractionWidget.h"


void UInteractionWidget::UpdateInteractionWidget(UInteractionComponent* InteractionComponent)
{
	OwningInteractionComponent = InteractionComponent;
	OnUpdateInteractionWidget();
}