// OutbreakZ: Mobile. All rights reserved.


#include "Player/Components/LadderClimbingComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/SurvivalCharacter.h"
#include "Player/Components/PlayerMovementComponent.h"
#include "World/LadderMaster.h"

ULadderClimbingComponent::ULadderClimbingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void ULadderClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<ASurvivalCharacter>(GetOwner());
}

void ULadderClimbingComponent::LadderInput(float Axis)
{
	if (FMath::IsNearlyZero(Axis))
	{
		LadderMoveDirection = ELadderMoveDirection::ELMD_None;
	}
	else if (Axis > 0.f)
	{
		LadderMoveDirection = ELadderMoveDirection::ELMD_MoveUp;
	}
	else
	{
		LadderMoveDirection = ELadderMoveDirection::ELMD_MoveDown;
	}
}

bool ULadderClimbingComponent::FindLadder()
{
	const FVector Start = PlayerRef->GetActorLocation();
	const FVector End = Start + UKismetMathLibrary::GetForwardVector(PlayerRef->GetActorRotation()) * 100.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult OutHit;

	const bool bHasHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start, End,
		TraceRadius,
		ObjectTypesArray, false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration, OutHit,
		true
	);

	if (bHasHit)
	{
		LadderEnterSide = ELadderEnterSide::ELES_FromTop;
		if (const auto Ladder = Cast<ALadderMaster>(OutHit.GetActor()))
		{
			LadderRef = Ladder;
			PlayerMovementComponent->ClimbToLadder();

			// const auto LadderStartTransform = Ladder->LowerStartPosition->GetComponentTransform();
			// const auto CapsuleHeight = PlayerRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			//
			// FLatentActionInfo LatentInfo;
			// LatentInfo.CallbackTarget = this;
			//
			// const auto LadderStartLocation = LadderStartTransform.GetLocation();
			// const auto MoveToLocation = FVector(
			// 	LadderStartLocation.X, LadderStartLocation.Y,
			// 	LadderStartLocation.Z) + FVector(0.f, 0.f, CapsuleHeight);
			//
			// UKismetSystemLibrary::MoveComponentTo(
			// 	PlayerRef->GetCapsuleComponent(),
			// 	MoveToLocation,
			// 	LadderStartTransform.GetRotation().Rotator(),
			// 	false, false,
			// 	MoveToOverTime,
			// 	false,
			// 	EMoveComponentAction::Move, LatentInfo);
		}
	}

	return bHasHit;
}

bool ULadderClimbingComponent::OnLadderCheck()
{
	return LadderRef != nullptr;
}

void ULadderClimbingComponent::Unclimb()
{
	LadderRef = nullptr;
	PlayerMovementComponent->UnclimbLadder();
}
