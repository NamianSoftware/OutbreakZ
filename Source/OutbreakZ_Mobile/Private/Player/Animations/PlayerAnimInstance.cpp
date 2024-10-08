// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animations/PlayerAnimInstance.h"

#include "Player/SurvivalCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/Components/LadderClimbingComponent.h"
#include "Player/Components/PlayerMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SetReferences();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!PlayerRef) return;

	DeltaTimeX = DeltaSeconds;

	SetEssentialMovementData();
	TrackMainStates();
}

void UPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
	if (!PlayerRef) return;

	UpdateCharacterTransform();
	ResetTransition();
}

UAnimSequence* UPlayerAnimInstance::GetPivotAnim(UAnimSequence* Pivot90LAnim, UAnimSequence* Pivot180LAnim,
                                                 UAnimSequence* Pivot90RAnim, UAnimSequence* Pivot180RAnim)
{
	const auto Acceleration = CharacterMovementRef->GetCurrentAcceleration();
	const auto NormalizedAcceleration = UKismetMathLibrary::Normal(Acceleration);
	const auto NormalizedVelocity = UKismetMathLibrary::Normal(Velocity);

	float DotProduct = FVector::DotProduct(NormalizedVelocity, NormalizedAcceleration);
	const auto Cross = UKismetMathLibrary::Cross_VectorVector(NormalizedVelocity, NormalizedAcceleration);

	const auto StartPivotAngle = UKismetMathLibrary::DegAcos(DotProduct);
	const auto bIsRight = Cross.Z > 0.f;

	if (StartPivotAngle > 90.f)
	{
		if (bIsRight)
		{
			return Pivot180RAnim;
		}
		return Pivot180RAnim;
	}

	if (bIsRight)
	{
		return Pivot90RAnim;
	}

	return Pivot90LAnim;
}

void UPlayerAnimInstance::SetReferences()
{
	if (const auto NewPlayerRef = Cast<ASurvivalCharacter>(TryGetPawnOwner()))
	{
		PlayerRef = NewPlayerRef;
		CharacterMovementRef = Cast<UPlayerMovementComponent>(PlayerRef->GetCharacterMovement());
	}
}

void UPlayerAnimInstance::SetEssentialMovementData()
{
	PrevVelocity = Velocity;
	Velocity = CharacterMovementRef->Velocity;
	bIsFalling = CharacterMovementRef->IsFalling();
	bIsCrouch = CharacterMovementRef->IsCrouching();
	bIsOnLadder = CharacterMovementRef->IsOnLadder();
	MaxSpeed = CharacterMovementRef->GetMaxSpeed();
	ActorRotation = PlayerRef->GetActorRotation();

	InputVector = CharacterMovementRef->GetLastInputVector();

	GroundSpeed = bIsFalling ? 0.f : Velocity.Size2D();

	DeterminateMainState();
	UpdateDisplacement();
	UpdatePivot();
}

void UPlayerAnimInstance::DeterminateMainState()
{
	PrevMainState = MainState;

	if (bIsOnLadder)
	{
		MainState = EMainState::EMS_OnLadder;
		return;
	}

	if (bIsFalling)
	{
		MainState = EMainState::EMS_OnAir;
		return;
	}

	MainState = EMainState::EMS_OnGround;
}

void UPlayerAnimInstance::UpdateAimOffset()
{
	auto const Normalized = UKismetMathLibrary::NormalizedDeltaRotator(PlayerRef->GetBaseAimRotation(),
	                                                                   PlayerRef->GetActorRotation());

	if (FMath::Abs(Normalized.Yaw) > AimYawLimit)
	{
		AimYaw = 0.f;
		AimPitch = 0.f;
	}
	else
	{
		AimYaw = Normalized.Yaw;
		AimPitch = Normalized.Pitch;
	}
}

void UPlayerAnimInstance::DetermineLocomotionState()
{
	PrevLocomotionState = LocomotionState;

	if (bIsFalling)
	{
		return;
	}

	TimeInLocomotionState += GetDeltaSeconds();
	// if (!UKismetMathLibrary::Greater_DoubleDouble(TimeInLocomotionState, MinTimeInLocomotionState)) return;

	DetermineGroundLocomotionState();
}

void UPlayerAnimInstance::TrackLocomotionStates()
{
	TrackLocomotionState(ELocomotionState::ELS_Idle, EntryFlags_LocomotionState.IdleFlag,
	                     &UPlayerAnimInstance::OnEntryIdle,
	                     &UPlayerAnimInstance::OnExitIdle,
	                     &UPlayerAnimInstance::WhileTrueIdle,
	                     &UPlayerAnimInstance::WhileFalseIdle);

	TrackLocomotionState(ELocomotionState::ELS_Walk, EntryFlags_LocomotionState.WalkFlag,
	                     &UPlayerAnimInstance::OnEntryWalk,
	                     &UPlayerAnimInstance::OnExitWalk,
	                     &UPlayerAnimInstance::WhileTrueWalk,
	                     &UPlayerAnimInstance::WhileFalseWalk);

	TrackLocomotionState(ELocomotionState::ELS_Jog, EntryFlags_LocomotionState.JogFlag,
	                     &UPlayerAnimInstance::OnEntryJog,
	                     &UPlayerAnimInstance::OnExitJog,
	                     &UPlayerAnimInstance::WhileTrueJog,
	                     &UPlayerAnimInstance::WhileFalseJog);

	TrackLocomotionState(ELocomotionState::ELS_Crouch, EntryFlags_LocomotionState.CrouchFlag,
	                     &UPlayerAnimInstance::OnEntryCrouch,
	                     &UPlayerAnimInstance::OnExitCrouch,
	                     &UPlayerAnimInstance::WhileTrueCrouch,
	                     &UPlayerAnimInstance::WhileFalseCrouch);
}

void UPlayerAnimInstance::UpdatePivot()
{
	const auto Acceleration = CharacterMovementRef->GetCurrentAcceleration();
	const auto Acceleration2D = Acceleration * FVector(1.f, 1.f, 0.f);
	const auto Velocity2D = Velocity * FVector(1.f, 1.f, 0.f);

	const auto NormalizedAcceleration2D = UKismetMathLibrary::Normal(Acceleration2D);
	const auto NormalizedVelocity2D = UKismetMathLibrary::Normal(Velocity2D);
	const auto DotNormalAccelerationToVelocity = UKismetMathLibrary::Dot_VectorVector(
		NormalizedAcceleration2D, NormalizedVelocity2D);
	bInPivot = DotNormalAccelerationToVelocity < 0.f;
}

void UPlayerAnimInstance::TrackMainStates()
{
	TrackMainState(EMainState::EMS_OnGround, EntryFlags_MainState.IsOnGround,
	               &UPlayerAnimInstance::OnEntryGroundState,
	               &UPlayerAnimInstance::OnExitGroundState,
	               &UPlayerAnimInstance::WhileTrueGroundState,
	               &UPlayerAnimInstance::WhileFalseGroundState);


	TrackMainState(EMainState::EMS_OnLadder, EntryFlags_MainState.IsOnLadder,
	               &UPlayerAnimInstance::OnEntryLadderState,
	               &UPlayerAnimInstance::OnExitLadderState,
	               &UPlayerAnimInstance::WhileTrueLadderState,
	               &UPlayerAnimInstance::WhileFalseLadderState);


	TrackMainState(EMainState::EMS_OnAir, EntryFlags_MainState.IsOnAir,
	               &UPlayerAnimInstance::OnEntryAirState,
	               &UPlayerAnimInstance::OnExitAirState,
	               &UPlayerAnimInstance::WhileTrueAirState,
	               &UPlayerAnimInstance::WhileFalseAirState);
}

void UPlayerAnimInstance::UpdateLean()
{
	const auto WorldDeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	const auto VelocitySubtraction = UKismetMathLibrary::Subtract_VectorVector(
		FVector(Velocity.X, Velocity.Y, 0), PrevVelocity);
	const auto Acceleration = VelocitySubtraction / WorldDeltaSeconds;

	const bool IsGainingMomentum = UKismetMathLibrary::Greater_DoubleDouble(
		UKismetMathLibrary::DotProduct2D(FVector2D(Acceleration), FVector2D(Velocity)), 0.f);

	const float MaxAcceleration = IsGainingMomentum
		                              ? CharacterMovementRef->GetMaxAcceleration()
		                              : CharacterMovementRef->GetMaxBrakingDeceleration();

	const auto ClampedAcceleration = UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcceleration);
	const auto RelativeAccelerationAmount = ActorRotation.UnrotateVector(ClampedAcceleration / MaxAcceleration);

	Lean = UKismetMathLibrary::VInterpTo(
		Lean,
		RelativeAccelerationAmount,
		WorldDeltaSeconds,
		LeanInterpSpeed
	);

	const auto LeanXPower = GetCurveValue(MoveDataLeanXName);
	const auto LeanYPower = GetCurveValue(MoveDataLeanYName);

	LeanX = Lean.X * LeanXPower;
	LeanY = Lean.Y * LeanYPower;
}

void UPlayerAnimInstance::ResetLean()
{
	LeanX = 0.f;
	LeanY = 0.f;
}

void UPlayerAnimInstance::UpdateCharacterTransform()
{
	if ((InLocomotionState() || InLandState())
		&& MainState == EMainState::EMS_OnGround)
	{
		MoveRotationBehavior();
	}
	else
	{
		ResetTargetRotations();
	}
}

void UPlayerAnimInstance::ResetTransition()
{
	bPlayStartAnim = false;
	bPlayLandAnim = false;
	bInPivot = false;
	bPlayGaitTransitionAnim = false;
}

FRotator UPlayerAnimInstance::GetTargetRotation() const
{
	float Yaw;
	if (bDoInputRotation)
	{
		Yaw = UKismetMathLibrary::MakeRotFromX(InputVector).Yaw;
	}
	else
	{
		Yaw = UKismetMathLibrary::MakeRotFromX(Velocity).Yaw;
	}
	return FRotator(0.f, Yaw, 0.f);
}

void UPlayerAnimInstance::MoveRotationBehavior()
{
	if (LocomotionState != ELocomotionState::ELS_Walk &&
		LocomotionState != ELocomotionState::ELS_Jog &&
		LocomotionState != ELocomotionState::ELS_Crouch)
		return;

	const auto TargetRotation = GetTargetRotation();

	PrimaryTargetRotation = UKismetMathLibrary::RInterpTo_Constant(PrimaryTargetRotation, TargetRotation, DeltaTimeX,
	                                                               PrimaryRotationInterpSpeed);
	SecondaryTargetRotation = UKismetMathLibrary::RInterpTo(SecondaryTargetRotation, PrimaryTargetRotation, DeltaTimeX,
	                                                        SecondaryRotationInterpSpeed);

	const auto RotationDelta = GetCurveValue(MoveDataRotationName);
	const auto RotationActive = GetCurveValue(MoveDataRotationActiveName);
	const auto CompleteRotationDelta = UKismetMathLibrary::FClamp(
		UKismetMathLibrary::SafeDivide(RotationDelta, RotationActive),
		0.f,
		1.f);

	const auto RotationOffset = StartAngle * CompleteRotationDelta;

	const FRotator NewRotation = FRotator(
		SecondaryTargetRotation.Pitch,
		SecondaryTargetRotation.Yaw - RotationOffset,
		SecondaryTargetRotation.Roll
	);

	PlayerRef->SetActorRotation(NewRotation);
}

void UPlayerAnimInstance::UpdateStop()
{
	if (PrevLocomotionState == ELocomotionState::ELS_Crouch)
	{
		StopAnim = CrouchStopAnim;
		AnimStartTime = CrouchAnimStartTime;
		PostStopLocomotionState = ELocomotionState::ELS_Crouch;
		return;
	}

	if (GroundSpeed > JogStopSpeedLimit)
	{
		StopAnim = JogStopAnim;
		AnimStartTime = JogAnimStartTime;
	}
	else
	{
		StopAnim = WalkStopAnim;
		AnimStartTime = WalkAnimStartTime;
	}

	PostStopLocomotionState = ELocomotionState::ELS_Idle;
}

void UPlayerAnimInstance::ResetTargetRotations()
{
	PrimaryTargetRotation = PlayerRef->GetActorRotation();
	SecondaryTargetRotation = PrimaryTargetRotation;
}

void UPlayerAnimInstance::UpdateLocomotionValues()
{
	const float SpeedCurveValue = GetCurveValue(MoveDataSpeedName);
	PlayRate = UKismetMathLibrary::SafeDivide(GroundSpeed, SpeedCurveValue);
}

void UPlayerAnimInstance::UpdateOnWalkEntry()
{
	UpdateEntryVariables();
	UpdateEntryVariables();
	UpdateStartAnim(WalkStartAnim,
	                WalkStart90LAnim, WalkStart90LAnimTime,
	                WalkStart180LAnim, WalkStart180LAnimTime,
	                WalkStart90RAnim, WalkStart90RAnimTime,
	                WalkStart180RAnim, WalkStart180RAnimTime,
	                WalkStartFAnim, WalkStartFAnimTime);
}

void UPlayerAnimInstance::UpdateOnJogEntry()
{
	UpdateEntryVariables();
	UpdateStartAnim(JogStartAnim,
	                JogStart90LAnim, JogStart90LAnimTime,
	                JogStart180LAnim, JogStart180LAnimTime,
	                JogStart90RAnim, JogStart90RAnimTime,
	                JogStart180RAnim, JogStart180RAnimTime,
	                JogStartFAnim, JogStartFAnimTime);
}

void UPlayerAnimInstance::UpdateOnCrouchEntry()
{
	UpdateEntryVariables();
	UpdateStartAnim(CrouchStartAnim,
	                CrouchStart90LAnim, CrouchStart90LAnimTime,
	                CrouchStart180LAnim, CrouchStart180LAnimTime,
	                CrouchStart90RAnim, CrouchStart90RAnimTime,
	                CrouchStart180RAnim, CrouchStart180RAnimTime,
	                CrouchStartFAnim, CrouchStartFAnimTime);
}

void UPlayerAnimInstance::UpdateEntryVariables()
{
	StartRotation = PlayerRef->GetActorRotation();

	PrimaryTargetRotation = UKismetMathLibrary::MakeRotFromX(InputVector);
	SecondaryTargetRotation = PrimaryTargetRotation;

	StartAngle = UKismetMathLibrary::NormalizedDeltaRotator(PrimaryTargetRotation, StartRotation).Yaw;
}

void UPlayerAnimInstance::UpdateDisplacement()
{
	const auto PrevWorldLocation = WorldLocation;
	WorldLocation = PlayerRef->GetActorLocation();
	DisplacementSinceLastUpdate = (WorldLocation - PrevWorldLocation).Size2D();
}

void UPlayerAnimInstance::UpdateCrouchTransitionAnim()
{
	switch (PrevLocomotionState)
	{
	case ELocomotionState::ELS_Walk:
		TransitionToCrouchAnim = WalkToCrouchAnim;
		AnimStartTime = FMath::Max(GetCurveValue(TransitionWalkToCrouchCurveName), WalkToCrouchAnimStartTime);
		break;
	case ELocomotionState::ELS_Jog:
		TransitionToCrouchAnim = JogToCrouchAnim;
		AnimStartTime = FMath::Max(GetCurveValue(TransitionJogToCrouchCurveName), JogToCrouchAnimStartTime);
		break;
	default:
		break;
	}
}

void UPlayerAnimInstance::UpdateWalkTransitionAnim()
{
	switch (PrevLocomotionState)
	{
	case ELocomotionState::ELS_Crouch:
		TransitionToWalkAnim = CrouchToWalkAnim;
		AnimStartTime = FMath::Max(GetCurveValue(TransitionWalkToCrouchCurveName), CrouchToWalkAnimStartTime);
		break;
	case ELocomotionState::ELS_Jog:
		TransitionToWalkAnim = JogToWalkAnim;
		AnimStartTime = JogToWalkAnimStartTime;
		break;
	default:
		break;
	}
}

void UPlayerAnimInstance::UpdateJogTransitionAnim()
{
	switch (PrevLocomotionState)
	{
	case ELocomotionState::ELS_Crouch:
		TransitionToJogAnim = CrouchToJogAnim;
		AnimStartTime = FMath::Max(GetCurveValue(TransitionJogToCrouchCurveName), CrouchToJogAnimStartTime);
		break;
	case ELocomotionState::ELS_Walk:
		TransitionToJogAnim = WalkToJogAnim;
		AnimStartTime = WalkToJogAnimStartTime;
		break;
	default:
		break;
	}
}

void UPlayerAnimInstance::DetermineGroundLocomotionState()
{
	if (IsPivoting()) return;

	const auto NormalizedVelocity = UKismetMathLibrary::Normal(Velocity);
	const auto NormalizedCurrentAcceleration = UKismetMathLibrary::Normal(
		CharacterMovementRef->GetCurrentAcceleration());

	const auto VelocityAccelerationDotProduct = FVector::DotProduct(NormalizedVelocity, NormalizedCurrentAcceleration);

	if (UKismetMathLibrary::Less_DoubleDouble(VelocityAccelerationDotProduct, -0.5))
	{
		LocomotionState = ELocomotionState::ELS_Idle;
		return;
	}

	if (bIsCrouch)
	{
		if (IsMovementWithinThresholds(CrouchMinCurrentSpeed, CrouchMinMaxSpeed, CrouchMinInputAcceleration))
		{
			LocomotionState = ELocomotionState::ELS_Crouch;
			return;
		}
	}
	else
	{
		if (IsMovementWithinThresholds(JogMinCurrentSpeed, JogMinMaxSpeed, JogMinInputAcceleration))
		{
			LocomotionState = ELocomotionState::ELS_Jog;
			return;
		}

		if (IsMovementWithinThresholds(WalkMinCurrentSpeed, WalkMinMaxSpeed, WalkMinInputAcceleration))
		{
			LocomotionState = ELocomotionState::ELS_Walk;
			return;
		}
	}

	LocomotionState = ELocomotionState::ELS_Idle;
}

void UPlayerAnimInstance::UpdateJumpAnims()
{
	bJumpFootIsRight = IsForwardRightFoot();

	switch (LocomotionState)
	{
	case ELocomotionState::ELS_Idle:
		StartJumpAnim = IdleStartJumpAnim;
		AnimStartTime = IdleStartJumpStartTime;
		FallingAnim = IdleFallingAnim;
		break;
	case ELocomotionState::ELS_Walk:
		if (bJumpFootIsRight)
		{
			StartJumpAnim = WalkStartJumpRAnim;
			AnimStartTime = WalkStartJumpRStartTime;
			FallingAnim = WalkRFallingAnim;
		}
		else
		{
			StartJumpAnim = WalkStartJumpLAnim;
			AnimStartTime = WalkStartJumpLStartTime;
			FallingAnim = WalkLFallingAnim;
		}
		break;
	case ELocomotionState::ELS_Jog:
		if (bJumpFootIsRight)
		{
			StartJumpAnim = JogStartJumpRAnim;
			AnimStartTime = JogStartJumpRStartTime;
			FallingAnim = JogRFallingAnim;
		}
		else
		{
			StartJumpAnim = JogStartJumpLAnim;
			AnimStartTime = JogStartJumpLStartTime;
			FallingAnim = JogLFallingAnim;
		}
		break;
	default: break;
	}
}

void UPlayerAnimInstance::UpdateLandAnim()
{
	if (bJumpFootIsRight)
	{
		JogLandAnim = JogRLandAnim;
		JogLandStartTime = JogRLandStartTime;

		WalkLandAnim = WalkRLandAnim;
		WalkLandStartTime = WalkRLandStartTime;
	}
	else
	{
		JogLandAnim = JogLLandAnim;
		JogLandStartTime = JogLLandStartTime;

		WalkLandAnim = WalkLLandAnim;
		WalkLandStartTime = WalkLLandStartTime;
	}
}

bool UPlayerAnimInstance::IsForwardRightFoot() const
{
	if (!PlayerRef) return false;

	const auto RootBoneLocation = GetOwningComponent()->GetSocketLocation(RootBoneName);
	const auto RFootBoneLocation = GetOwningComponent()->GetSocketLocation(RFootBoneName);

	const auto RootRFootRotation = UKismetMathLibrary::FindLookAtRotation(RootBoneLocation, RFootBoneLocation);
	const auto RootRFootForwardVector = UKismetMathLibrary::GetForwardVector(RootRFootRotation);

	const auto ControlForward = UKismetMathLibrary::GetForwardVector(PlayerRef->GetControlRotation());

	const auto ControlVelocityRotation = UKismetMathLibrary::FindLookAtRotation(ControlForward, Velocity);
	const auto ControlVelocityForwardVector = UKismetMathLibrary::GetForwardVector(ControlVelocityRotation);

	return UKismetMathLibrary::Dot_VectorVector(RootRFootForwardVector, ControlVelocityForwardVector) > 0.f;
}

#pragma region LOCOMOTION_STATE_CALLBACKS
#pragma region IDLE
void UPlayerAnimInstance::OnEntryIdle()
{
	ResetTargetRotations();
	UpdateStop();
}

void UPlayerAnimInstance::OnExitIdle()
{
}

void UPlayerAnimInstance::WhileTrueIdle()
{
}

void UPlayerAnimInstance::WhileFalseIdle()
{
}
#pragma endregion

#pragma region WALK
void UPlayerAnimInstance::OnEntryWalk()
{
	if (IsPivoting()) return;

	if (PrevLocomotionState == ELocomotionState::ELS_Idle)
	{
		UpdateOnWalkEntry();
		bPlayStartAnim = true;
	}
	else
	{
		if (UKismetMathLibrary::Less_DoubleDouble(GroundSpeed, MaxSpeedForPlayingStartWalkAnim))
		{
			UpdateOnWalkEntry();
			bPlayStartAnim = true;
		}
		else
		{
			bPlayGaitTransitionAnim = true;
			UpdateWalkTransitionAnim();

			ResetTargetRotations();
		}
	}
}

void UPlayerAnimInstance::OnExitWalk()
{
}

void UPlayerAnimInstance::WhileTrueWalk()
{
	UpdateLocomotionValues();
}

void UPlayerAnimInstance::WhileFalseWalk()
{
}
#pragma endregion

#pragma region JOG
void UPlayerAnimInstance::OnEntryJog()
{
	if (PrevLocomotionState == ELocomotionState::ELS_Idle)
	{
		UpdateOnJogEntry();
		bPlayStartAnim = true;
	}
	else
	{
		if (UKismetMathLibrary::Less_DoubleDouble(GroundSpeed, MaxSpeedForPlayingStartJogAnim))
		{
			UpdateOnJogEntry();
			bPlayStartAnim = true;
		}
		else
		{
			bPlayGaitTransitionAnim = true;
			UpdateJogTransitionAnim();

			ResetTargetRotations();
		}
	}
}

void UPlayerAnimInstance::OnExitJog()
{
}

void UPlayerAnimInstance::WhileTrueJog()
{
	UpdateLocomotionValues();
}

void UPlayerAnimInstance::WhileFalseJog()
{
}
#pragma endregion

#pragma region CROUCH
void UPlayerAnimInstance::OnEntryCrouch()
{
	if (PrevLocomotionState == ELocomotionState::ELS_Idle)
	{
		UpdateOnCrouchEntry();
		bPlayStartAnim = true;
	}
	else
	{
		if (UKismetMathLibrary::Less_DoubleDouble(GroundSpeed, MaxSpeedForPlayingStartWalkAnim))
		{
			UpdateOnCrouchEntry();
			bPlayStartAnim = true;
		}
		else
		{
			bPlayGaitTransitionAnim = true;
			UpdateCrouchTransitionAnim();
		}
	}
}

void UPlayerAnimInstance::OnExitCrouch()
{
}

void UPlayerAnimInstance::WhileTrueCrouch()
{
	UpdateLocomotionValues();
}

void UPlayerAnimInstance::WhileFalseCrouch()
{
}
#pragma endregion

#pragma region MAIN_STATE_CALLBACKS
#pragma region ON_GROUND
void UPlayerAnimInstance::OnEntryGroundState()
{
	if (PrevMainState == EMainState::EMS_OnAir)
	{
		bPlayLandAnim = true;
	}

	ResetLean();
}

void UPlayerAnimInstance::OnExitGroundState()
{
	ResetLean();
}

void UPlayerAnimInstance::WhileTrueGroundState()
{
	UpdateLean();
	UpdateAimOffset();
	DetermineLocomotionState();
	TrackLocomotionStates();
}

void UPlayerAnimInstance::WhileFalseGroundState()
{
}
#pragma endregion

#pragma region ON_LADDER
void UPlayerAnimInstance::OnEntryLadderState()
{
	UpdateEnterSideLadder();
}

void UPlayerAnimInstance::OnExitLadderState()
{
}

void UPlayerAnimInstance::WhileTrueLadderState()
{
	UpdateAimOffset();
}

void UPlayerAnimInstance::WhileFalseLadderState()
{
}
#pragma endregion

#pragma region ON_AIR
void UPlayerAnimInstance::OnEntryAirState()
{
	UpdateJumpAnims();
}

void UPlayerAnimInstance::OnExitAirState()
{
	UpdateLandAnim();
}

void UPlayerAnimInstance::WhileTrueAirState()
{
	UpdateAimOffset();
}

void UPlayerAnimInstance::WhileFalseAirState()
{
}
#pragma endregion
#pragma endregion


void UPlayerAnimInstance::UpdateEnterSideLadder()
{
	// switch (PlayerRef->GetLadderClimbingComponent()->GetLadderEnterSide())
	// {
	// case ELadderEnterSide::ELES_FromTop:
	// 	EnterLadderAnim = LadderEnterTopAnim;
	// 	AnimStartTime = LadderEnterTopStartTime;
	// 	break;
	//
	// case ELadderEnterSide::ELES_FromBottom:
	// 	EnterLadderAnim = LadderEnterBottomAnim;
	// 	AnimStartTime = LadderEnterBottomStartTime;
	// 	break;
	// }
}

#pragma endregion
#pragma endregion

bool UPlayerAnimInstance::IsMovementWithinThresholds(float MinCurrentSpeed, float MinMaxSpeed,
                                                     float MinInputAcceleration) const
{
	return UKismetMathLibrary::LessEqual_DoubleDouble(MinCurrentSpeed, GroundSpeed) &&
		UKismetMathLibrary::LessEqual_DoubleDouble(MinMaxSpeed, MaxSpeed) &&
		UKismetMathLibrary::LessEqual_DoubleDouble(MinInputAcceleration, InputVector.Size());
}

void UPlayerAnimInstance::TrackLocomotionState(ELocomotionState TracedState, bool& EnterFlag,
                                               void (UPlayerAnimInstance::*OnEnterCallback)(),
                                               void (UPlayerAnimInstance::*OnExitCallback)(),
                                               void (UPlayerAnimInstance::*WhileTrueCallback)(),
                                               void (UPlayerAnimInstance::*WhileFalseCallback)())
{
	if (LocomotionState == TracedState)
	{
		if (EnterFlag != true)
		{
			EnterFlag = true;
			(this->*OnEnterCallback)();
			TimeInLocomotionState = 0.f;
		}
		else
		{
			(this->*WhileTrueCallback)();
		}
	}
	else
	{
		if (EnterFlag != false)
		{
			EnterFlag = false;
			(this->*OnExitCallback)();
		}
		else
		{
			(this->*WhileFalseCallback)();
		}
	}
}

void UPlayerAnimInstance::TrackMainState(EMainState TracedState, bool& EnterFlag,
                                         void (UPlayerAnimInstance::*OnEnterCallback)(),
                                         void (UPlayerAnimInstance::*OnExitCallback)(),
                                         void (UPlayerAnimInstance::*WhileTrueCallback)(),
                                         void (UPlayerAnimInstance::*WhileFalseCallback)())
{
	if (MainState == TracedState)
	{
		if (EnterFlag != true)
		{
			EnterFlag = true;
			(this->*OnEnterCallback)();
		}
		else
		{
			(this->*WhileTrueCallback)();
		}
	}
	else
	{
		if (EnterFlag != false)
		{
			EnterFlag = false;
			(this->*OnExitCallback)();
		}
		else
		{
			(this->*WhileFalseCallback)();
		}
	}
}

void UPlayerAnimInstance::UpdateStartAnim(UAnimSequence*& FinishAnim, UAnimSequence* Start90LAnim,
                                          float Start90LAnimTime, UAnimSequence* Start180LAnim, float Start180LAnimTime,
                                          UAnimSequence* Start90RAnim,
                                          float Start90RAnimTime, UAnimSequence* Start180RAnim, float Start180RAnimTime,
                                          UAnimSequence* StartFAnim,
                                          float StartFAnimTime)
{
	if (UKismetMathLibrary::InRange_FloatFloat(StartAngle, -135, -45, false, true))
	{
		FinishAnim = Start90LAnim;
		AnimStartTime = Start90LAnimTime;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(StartAngle, -180, -135, true, true))
	{
		FinishAnim = Start180LAnim;
		AnimStartTime = Start180LAnimTime;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(StartAngle, 45, 135, true, false))
	{
		FinishAnim = Start90RAnim;
		AnimStartTime = Start90RAnimTime;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(StartAngle, 135, 180, true, true))
	{
		FinishAnim = Start180RAnim;
		AnimStartTime = Start180RAnimTime;
	}
	else
	{
		FinishAnim = StartFAnim;
		AnimStartTime = StartFAnimTime;
	}
}
