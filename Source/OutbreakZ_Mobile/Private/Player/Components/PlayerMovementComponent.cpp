// OutbreakZ: Mobile. All rights reserved.


#include "Player/Components/PlayerMovementComponent.h"

#include "GameFramework/Character.h"


#pragma region SAVED_DATA
UPlayerMovementComponent::FSavedMove_CMP::FSavedMove_CMP()
{
	Saved_bWantsToJog = 0;
}

bool UPlayerMovementComponent::FSavedMove_CMP::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                                                float MaxDelta) const
{
	const FSavedMove_CMP* NewCMPMove = static_cast<FSavedMove_CMP*>(NewMove.Get());

	if (NewCMPMove->Saved_bWantsToJog != Saved_bWantsToJog) return false;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UPlayerMovementComponent::FSavedMove_CMP::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToJog = 0;
}

uint8 UPlayerMovementComponent::FSavedMove_CMP::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToJog) Result |= FLAG_Custom_1;

	return Result;
}

void UPlayerMovementComponent::FSavedMove_CMP::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                                            FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const auto CharacterMovement = Cast<UPlayerMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToJog = CharacterMovement->Safe_bWantsToJog;
}

void UPlayerMovementComponent::FSavedMove_CMP::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	const auto CharacterMovement = Cast<UPlayerMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->Safe_bWantsToJog = Saved_bWantsToJog;
}
#pragma endregion

#pragma region CLIENT_NETWORK_PREDICTION_DATA
UPlayerMovementComponent::FNetworkPredictionData_Client_CMP::FNetworkPredictionData_Client_CMP(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UPlayerMovementComponent::FNetworkPredictionData_Client_CMP::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CMP());
}
#pragma endregion

UPlayerMovementComponent::UPlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;

	BrakingFrictionFactor = 0.f;
	GroundFriction = 1.f;

	MaxWalkSpeedCrouched = 165.f;

	AirControl = 0.f;
	AirControlBoostMultiplier = 0.f;
	AirControlBoostVelocityThreshold = 0.f;
}

FNetworkPredictionData_Client* UPlayerMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner);

	if (ClientPredictionData == nullptr)
	{
		UPlayerMovementComponent
			* MutableThis = const_cast<UPlayerMovementComponent
				*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CMP(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UPlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToJog = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

void UPlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                   const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
		
	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToJog)
		{
			if(AppliedMovementState != EAppliedMovementState::EAMS_Jog)
			{
				ApplyMovementConfig(JogConfig);
				AppliedMovementState = EAppliedMovementState::EAMS_Jog;
			}
		}
		else if (AppliedMovementState != EAppliedMovementState::EAMS_Walk)
		{
			ApplyMovementConfig(WalkConfig);
			AppliedMovementState = EAppliedMovementState::EAMS_Walk;
		}
	}
}

void UPlayerMovementComponent::StartJog()
{
	if (IsCrouching())
	{
		StopCrouch();
	}
	
	Safe_bWantsToJog = true;
}

void UPlayerMovementComponent::StopJog()
{
	Safe_bWantsToJog = false;
}

void UPlayerMovementComponent::ToggleCrouch()
{
	bWantsToCrouch = !bWantsToCrouch;
}

void UPlayerMovementComponent::StartCrouch()
{
	bWantsToCrouch = true;
}

void UPlayerMovementComponent::StopCrouch()
{
	bWantsToCrouch = false;
}

void UPlayerMovementComponent::ClimbToLadder()
{
	SetMovementMode(MOVE_Flying);
	Velocity = FVector::Zero();
	bIsOnLadder = true;
}

void UPlayerMovementComponent::UnclimbLadder()
{
	SetMovementMode(MOVE_Walking);
	bIsOnLadder = false;
}

void UPlayerMovementComponent::ApplyMovementConfig(const FMovementStateConfig& MovementConfig)
{
	MaxWalkSpeed = MovementConfig.MaxWalkSpeed;
	MaxAcceleration = MovementConfig.MaxAcceleration;
	BrakingDecelerationWalking = MovementConfig.BrakingDeceleration;
}
