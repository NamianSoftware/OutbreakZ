// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionComponent.generated.h"


class UWidgetComponent;
class ASurvivalCharacter;
class UInteractionWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteraction, ASurvivalCharacter*, Character);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OUTBREAKZ_MOBILE_API UInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float InteractionTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float InteractionDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float InteractionAngle;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	// UTexture2D* InteractTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	bool bAllowMultipleInteractors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	bool bNeedFocusOutline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	UAnimMontage* InteractionAnimation;

public:
	UPROPERTY(EditAnywhere, Category="Interaction")
	UWidgetComponent* InteractionWidgetComponent;
	
	UPROPERTY()
	bool bIsFocus = false;
	
public:	
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void Deactivate() override;

	bool CanInteract(ASurvivalCharacter* Character);

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
	
	UPROPERTY()
	TArray<ASurvivalCharacter*> Interactors;

public:
	/* DELEGATES */
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnShowWidget;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnUnShowWidget;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnBeginFocus;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnEndFocus;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnBeginInteract;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnEndInteract;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteraction OnInteract;

public:
	void RefreshWidget();
	
	void ShowWidget(ASurvivalCharacter* Character);
	void UnShowWidget(ASurvivalCharacter* Character);
	
	void BeginFocus(ASurvivalCharacter* Character);
	void EndFocus(ASurvivalCharacter* Character);

	void BeginInteract(ASurvivalCharacter* Character);
	void EndInteract(ASurvivalCharacter* Character);

	void Interact(ASurvivalCharacter* Character);

	// UFUNCTION(BlueprintCallable)
	// void SetInteractImage(UTexture2D* NewImage);

	UFUNCTION(BlueprintPure, Category="Interaction")
	float GetInteractPercentage();

	// UFUNCTION(BlueprintCallable, Category="Interaction")
	// void SetInteractableNameText(const FText& NewNameText);
	//
	// UFUNCTION(BlueprintCallable, Category="Interaction")
	// void SetInteractableActionText(const FText& NewActionText);

	UFUNCTION(BlueprintPure, Category="Interaction")
	FORCEINLINE bool GetIsFocus() const { return bIsFocus; }
		
};
