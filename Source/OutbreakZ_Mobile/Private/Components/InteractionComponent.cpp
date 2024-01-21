// OutbreakZ: Mobile. All rights reserved.


#include "Components/InteractionComponent.h"

#include "SurvivalCharacter.h"
#include "UI/Widgets/Components/InteractionWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


UInteractionComponent::UInteractionComponent()
{
	UActorComponent::SetComponentTickEnabled(false);

	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	InteractionAngle = 45.f;
	bAllowMultipleInteractors = true;
	bNeedFocusOutline = false;

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("InteractionWidget");
	InteractionWidgetComponent->SetupAttachment(this);
	InteractionWidgetComponent->SetHiddenInGame(true);
	
	UActorComponent::SetActive(true);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UInteractionWidget* InteractWidget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
	{
		InteractionWidget = InteractWidget;
		
		OnShowWidget.AddDynamic(InteractionWidget, &UInteractionWidget::OnShowWidget);
		OnUnShowWidget.AddDynamic(InteractionWidget, &UInteractionWidget::OnUnShowWidget);
		OnBeginFocus.AddDynamic(InteractionWidget, &UInteractionWidget::OnBeginFocusWidget);
		OnEndFocus.AddDynamic(InteractionWidget, &UInteractionWidget::OnEndFocusWidget);
	}
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();
	
	for(const auto Interactor : Interactors)
	{
		EndFocus(Interactor);
		EndInteract(Interactor);
	}

	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ASurvivalCharacter* Character)
{
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() > 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() && Character;
}

void UInteractionComponent::RefreshWidget()
{
	if(!bHiddenInGame)
	{
		if(InteractionWidget)
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

void UInteractionComponent::ShowWidget(ASurvivalCharacter* Character)
{
	if(!IsActive() || !GetOwner() || !Character || !bHiddenInGame) return;
	
	OnShowWidget.Broadcast(Character);
	
	SetHiddenInGame(false);
	
	RefreshWidget();
}

void UInteractionComponent::UnShowWidget(ASurvivalCharacter* Character)
{
	if(bHiddenInGame) return;
	
	OnUnShowWidget.Broadcast(Character);
	
	SetHiddenInGame(true);
}

void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
	if(bIsFocus) return;

	bIsFocus = true;

	if(Character->IsLocallyControlled() && bNeedFocusOutline)
	{
		TArray<UActorComponent*> VisualComponents;
		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), VisualComponents);
		for(const auto& VisualComp : VisualComponents)
		{
			if(UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(VisualComp))
			{
				Primitive->SetCustomDepthStencilValue(1);
			}
		}
	}
	
	OnBeginFocus.Broadcast(Character);
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{
	if(!bIsFocus) return;

	bIsFocus = false;
	
	if(Character->IsLocallyControlled() && bNeedFocusOutline)
	{
		TArray<UActorComponent*> VisualComponents;
		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), VisualComponents);
		for(const auto& VisualComp : VisualComponents)
		{
			if(UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(VisualComp))
			{
				Primitive->SetCustomDepthStencilValue(0);
			}
		}
	}
	
	OnEndFocus.Broadcast(Character);
}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{
	if(CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
	if(CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

// void UInteractionComponent::SetInteractImage(UTexture2D* NewImage)
// {
	// InteractTexture = NewImage;
	// RefreshWidget();
// }

float UInteractionComponent::GetInteractPercentage()
{
	if(Interactors.IsValidIndex(0))
	{
	// 	const int32 InteractorIndex = Interactors.Find(Cast<ASurvivalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)));
	//
	// 	if(InteractorIndex == -1) return 0.f;
	// 	
	// 	if(const auto Interactor = Interactors[InteractorIndex])
	// 	{
	// 		if(Interactor && Interactor->IsInteracting())
	// 		{
	// 			return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
	// 		}
	// 	}
	}

	return 0.f;
}

