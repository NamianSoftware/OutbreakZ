// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/MantlingSystemComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/CollisionProfile.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"

UMantlingSystemComponent::UMantlingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsEnabled = true;

	Start_High = 0.5f;
	Start_Low = 0.5f;

	FwdOffset = 45.0f;
	HighOffset = 60.0f;
	LowOffset = 60.0f;

	DistanceToGrab = -40.0f;
	MantleEnterThreshold = 5.0f;

	//MantleFwdAmount_High = 1.0f;
	//MantleUpAmount_High = 1.0f;

	//MantleFwdAmount_Low = 1.0f;
	//MantleUpAmount_Low = 1.0f;

	//EndOffset_Up = 2.5f;
	//EndOffset_Fwd = 20.0f;

	Pelvis = TEXT("Pelvis");

	bHit = false;

	MontageInstanceID = INDEX_NONE;
}

/*
float UMantlingSystemComponent::GetMontagePos(UAnimMontage* Montage)
{
	AnimInstance = Mesh->GetAnimInstance();
	float MontagePos = AnimInstance->Montage_GetPosition(Montage);

	return MontagePos;
}
*/

/*
void UMantlingSystemComponent::UnbindDelegates()
{
	if (AnimInstance)
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UMantlingSystemComponent::OnNotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UMantlingSystemComponent::OnNotifyEnd);
	}
}
*/

bool UMantlingSystemComponent::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void UMantlingSystemComponent::LineTrace(FHitResult& HitResult, FVector Start, FVector End)
{
	const ECollisionChannel CollisionChannel = ECollisionChannel::ECC_GameTraceChannel1;
		//UCollisionProfile::Get()->ConvertToCollisionChannel(true, MantleTraceChannel); // TraceTypeQuery1

	ActorsToIgnore.Add(Char);

	FCollisionQueryParams Params(FName("Line Trace Single"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;
	Params.AddIgnoredActors(ActorsToIgnore);

	FGenericPhysicsInterface::RaycastSingle(Char->GetWorld(), HitResult, Start, End, CollisionChannel, Params, FCollisionResponseParams::DefaultResponseParam, FCollisionObjectQueryParams::DefaultObjectQueryParam);

	if(bIsDebug)
	{
		DrawDebugLine(
					GetWorld(),
					Start,
					End,
					FColor(255, 0, 0),
					false, -1, 0,
					12.333
				);
	}
}

void UMantlingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Actor = GetOwner();
	Char = Cast<ACharacter>(Actor);
	Mesh = Char ->GetMesh();
	Capsule = Char->GetCapsuleComponent();
	CharMov = Char->GetCharacterMovement();
	CapsuleHH = Capsule->GetScaledCapsuleHalfHeight();
	
	AnimInstance = Mesh->GetAnimInstance();

	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UMantlingSystemComponent::OnNotifyBegin);
	AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UMantlingSystemComponent::OnNotifyEnd);
	AnimInstance->OnMontageEnded.AddDynamic(this, &UMantlingSystemComponent::OnMontageEnded);
}

void UMantlingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	const bool bIsInAir = CharMov->IsFalling();

	// FVector Acceleration = CharMov->GetCurrentAcceleration();
	// float AccelerationSize = Acceleration.Size();

	// bool bIsAccelerating = AccelerationSize > 0.0f ? true : false;

	FVector ActorLoc = Actor->GetActorLocation();

	FHitResult HitResult;

	if (bIsEnabled && bIsInAir) /*&& bIsAccelerating*/
	{
		FVector ActorForwardLoc = UKismetMathLibrary::GetForwardVector(Actor->GetActorRotation());
		
		FVector Start;
		FVector End;

		Start = ActorLoc;
		Start += (ActorForwardLoc * FwdOffset); 

		End = Start;
		End.Z -= (HighOffset);
		Start.Z += CapsuleHH + HighOffset;

		LineTrace(HitResult, Start, End);

		bHit = HitResult.bBlockingHit;
	}
	else
	{
		bHit = false;

		bDoOnce_MantleEnter = true;
	}

	if (bHit && Actor->HasAuthority())
	{
		if ((HitResult.Location.Z - ActorLoc.Z) >= 0.0f)
		{
			float HitLocZ = HitResult.Location.Z;

			float DistanceToGrab_Dynamic = HitLocZ - (ActorLoc.Z + CapsuleHH + DistanceToGrab);

			if (abs(DistanceToGrab_Dynamic) < MantleEnterThreshold)
			{
				if (bDoOnce_MantleEnter)
				{
					bDoOnce_MantleEnter = false;

					FVector ActorLoc_Mantle = Actor->GetActorLocation();
					FRotator RootRot_Mantle = Mesh->GetSocketRotation(Pelvis);

					Multi_Mantle(HitResult.GetActor(), ActorLoc_Mantle, RootRot_Mantle, HitResult.Location + ZOffset, Montage_High);
				}
			}
		}
		else if ((HitResult.Location.Z - ActorLoc.Z) < 0.0f)
		{
			float HitLocZ = HitResult.Location.Z;

			if (bDoOnce_MantleEnter)
			{
				bDoOnce_MantleEnter = false;

				FVector ActorLoc_Mantle = Actor->GetActorLocation();
				FRotator RootRot_Mantle = Mesh->GetSocketRotation(Pelvis);

				Multi_Mantle(HitResult.GetActor(), ActorLoc_Mantle, RootRot_Mantle, HitResult.Location + ZOffset, Montage_Low);
			}
		}
	}

	if (bSetActorLoc)
	{
		float NotifyBegin;
		float NotifyDuration;

		if (bHighMantle)
		{
			NotifyBegin = NotifyBegin_High;
			NotifyDuration = NotifyDuration_High;
		}
		else
		{
			NotifyBegin = NotifyBegin_Low;
			NotifyDuration = NotifyDuration_Low;
		}

		Mantle_RootRot.Yaw += Mantle_RootRot.Yaw < 0.0f ? 360.0f : 0.0f;

		FVector NewHitLoc = HitLoc;
		NewHitLoc.Z = HitLoc.Z + CapsuleHH;
		FVector FwdVector = NewHitLoc - MantleBegin_ActorLoc; //Mantle_RootRot.Vector();
		FVector FwdUnit = FwdVector / FwdVector.Size();



		UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();

		float MontagePos = AnimInstance->Montage_GetPosition(Montage);
		float TimeRemaining = NotifyBegin + NotifyDuration - MontagePos;

		//TimeRemaining = TimeRemaining < DeltaTime ? DeltaTime : TimeRemaining;

		float n = TimeRemaining / DeltaTime;

		float DistanceToHit = (NewHitLoc - Mantle_ActorLoc).Size();

		float DistanceThisFrame =  DistanceToHit / n;

		DistanceThisFrame = DistanceThisFrame > DistanceToHit ? DistanceToHit : DistanceThisFrame;

		//FVector MantleVec = FwdUnit * MantleUpAmount;
		FVector MantleVec = FwdUnit * (FwdVector.Size() / (NotifyDuration/DeltaTime));

		//MantleVec.X = FwdUnit.X * MantleFwdAmount;
		//MantleVec.Y = FwdUnit.Y * MantleFwdAmount;
		//MantleVec.Z = MantleUpAmount;

		Mantle_ActorLoc += DistanceThisFrame * FwdUnit;

		Actor->SetActorLocation(Mantle_ActorLoc, false, nullptr, ETeleportType::TeleportPhysics);

		/*
		UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();

		if (Montage)
		{
			float MontagePos = AnimInstance->Montage_GetPosition(Montage);
			float TimeRemaining = AnimNotifyBegin + AnimNotifyDuration - MontagePos;

			//ActorLoc = Actor->GetActorLocation();

			FVector NewHitLoc = HitLoc;
			NewHitLoc.Z +=  CapsuleHH;

			FVector ActorToHit =  NewHitLoc - MantleBegin_ActorLoc;
			float ActorToHitSize = ActorToHit.Size();
			FVector ActorToHitUnit = ActorToHit / ActorToHitSize;

			float n = TimeRemaining / DeltaTime;

			//float DistanceThisFrame = ActorToHitSize / n;

			float DistanceThisFrame = ActorToHitSize * 0.1f;

			FVector NewActorLoc = ActorLoc + ActorToHitUnit * DistanceThisFrame;
			MantleBegin_ActorLoc = NewActorLoc;

			Actor->SetActorLocation(NewActorLoc, false, nullptr, ETeleportType::TeleportPhysics);

			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), ActorToHitSize));
		}
		*/
		
		//UE_LOG(LogTemp, Warning, TEXT("%f"), n);

	}
}

void UMantlingSystemComponent::Multi_Mantle_Implementation(AActor* InHitActor, FVector InLoc, FRotator InRot, FVector InHitLoc,
	UAnimMontage* MantleMontage)
{
	float BeginTime;

	if (MantleMontage == Montage_High)
	{
		bHighMantle = true;
		BeginTime = Start_High;
	}
	else
	{
		bHighMantle = false;
		BeginTime = Start_Low;
	}

	HitActor = InHitActor;
	HitLoc = InHitLoc;

	MantleBegin_ActorLoc = InLoc;
	Mantle_ActorLoc = InLoc;
	Mantle_RootRot = InRot;

	CharMov->DisableMovement();

	AnimInstance->Montage_Play(MantleMontage, 1.0f, EMontagePlayReturnType::MontageLength, BeginTime);

	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MantleMontage);
	MontageInstanceID = MontageInstance->GetInstanceID();

	bIsMantling = true;
	OnMantleStateUpdate.Broadcast(bIsMantling);
}

void UMantlingSystemComponent::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		if (HitActor)
		{
			//HitActor->SetActorEnableCollision(false);
			Capsule->IgnoreActorWhenMoving(HitActor, true);
			bSetActorLoc = true;
		}
	}
}

void UMantlingSystemComponent::OnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		if (HitActor)
		{
			//FVector FwdVector = HitLoc - MantleBegin_ActorLoc; //Mantle_RootRot.Vector();
			//FVector FwdUnit = FwdVector / FwdVector.Size();

			//FVector EndActorLoc = Actor->GetActorLocation();
			//EndActorLoc = HitLoc;
			//EndActorLoc.Z += CapsuleHH + EndOffset_Up;
			//EndActorLoc.Y += FwdUnit.Y * EndOffset_Fwd;
			//EndActorLoc.X += FwdUnit.X * EndOffset_Fwd;
			//Actor->SetActorLocation(EndActorLoc, false, nullptr, ETeleportType::TeleportPhysics);

			//HitActor->SetActorEnableCollision(true);
			Capsule->IgnoreActorWhenMoving(HitActor, false);
			HitActor = nullptr;
			bSetActorLoc = false;
		}
	}
}

void UMantlingSystemComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		CharMov->SetMovementMode(EMovementMode::MOVE_Walking, 0);

		bIsMantling = false;
		OnMantleStateUpdate.Broadcast(bIsMantling);
	}

	//UnbindDelegates();
}