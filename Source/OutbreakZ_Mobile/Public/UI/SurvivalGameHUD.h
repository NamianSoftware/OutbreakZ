// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SurvivalGameHUD.generated.h"


UCLASS()
class OUTBREAKZ_MOBILE_API ASurvivalGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASurvivalGameHUD();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
	UUserWidget* HUDWidget;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
};
