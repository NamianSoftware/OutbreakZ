// OutbreakZ: Mobile. All rights reserved.


#include "UI/SurvivalGameHUD.h"

#include "Blueprint/UserWidget.h"

ASurvivalGameHUD::ASurvivalGameHUD()
{
}

void ASurvivalGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if(HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
}
