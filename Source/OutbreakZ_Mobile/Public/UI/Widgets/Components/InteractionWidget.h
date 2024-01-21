// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"


class ASurvivalCharacter;
class UInteractionComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionWidget);


UCLASS()
class OUTBREAKZ_MOBILE_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void UpdateInteractionWidget(UInteractionComponent* InteractionComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInteractionWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowWidget(ASurvivalCharacter* Character);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnShowWidget(ASurvivalCharacter* Character);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEndFocusWidget(ASurvivalCharacter* Character);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginFocusWidget(ASurvivalCharacter* Character);

	UPROPERTY(BlueprintReadOnly, Category="Interaction", meta=(ExposeOnSpawn))
	UInteractionComponent* OwningInteractionComponent;
	
};
