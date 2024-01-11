// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	ELS_Idle UMETA(DisplayName = "Idle"),
	ELS_Walk UMETA(DisplayName = "Walk"),
	ELS_Jog UMETA(DisplayName = "Jog"),
	ELS_Crouch UMETA(DisplayName = "Crouch")
};


UENUM(BlueprintType)
enum class EMainState : uint8
{
	EMS_OnGround UMETA(DisplayName = "OnGround"),
	EMS_OnLadder UMETA(DisplayName = "OnLadder"),
	EMS_OnAir UMETA(DisplayName = "OnAir"),
	EMS_OnVehicle UMETA(DisplayName = "OnVehicle"),
};


struct OnEntryFlags_LocomotionState
{
	bool IdleFlag = true;
	bool WalkFlag = false;
	bool JogFlag = false;
	bool CrouchFlag = false;
};


struct OnEntryFlags_MainState
{
	bool IsOnGround = true;
	bool IsOnLadder = false;
	bool IsOnAir = false;
	bool IsOnVehicle = false;
};


UCLASS()
class OUTBREAKZ_MOBILE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool InLocomotionState();
	
	UFUNCTION(BlueprintImplementableEvent)
	bool InLandState();

#pragma region REFERENCES

private:
	void SetReferences();

protected:
	UPROPERTY(BlueprintReadOnly, Category="References")
	class ASurvivalCharacter* PlayerRef;

	UPROPERTY(BlueprintReadOnly, Category="References")
	class UPlayerMovementComponent* CharacterMovementRef;
#pragma endregion

#pragma region ESSENTIAL_DATA

private:
	void SetEssentialMovementData();
	void DeterminateMainState();
	void UpdateAimOffset();
	void DetermineLocomotionState();
	void TrackLocomotionStates();
	
	void TrackMainStates();

	void UpdateLean();
	void ResetLean();
	
	void UpdateCharacterTransform();
	void ResetTransition();
	FRotator GetTargetRotation() const;

	void MoveRotationBehavior();
	void UpdateStop();

	void ResetTargetRotations();

	void UpdateLocomotionValues();
	void UpdateOnWalkEntry();
	void UpdateOnJogEntry();
	void UpdateOnCrouchEntry();
	void UpdateEntryVariables();

	void UpdateCrouchTransitionAnim();
	void UpdateWalkTransitionAnim();
	void UpdateJogTransitionAnim();

	void DetermineGroundLocomotionState();

	void UpdateJumpAnims();
	void UpdateLandAnim();
	
	UFUNCTION(BlueprintPure)
	bool IsForwardRightFoot() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	float DeltaTimeX;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	FVector PrevVelocity;
	
	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	bool bIsCrouch;
	
	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	bool bIsOnLadder;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	float MaxSpeed;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	FRotator ActorRotation;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	FVector InputVector;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	ELocomotionState PrevLocomotionState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	ELocomotionState LocomotionState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	ELocomotionState LandLocomotionState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	EMainState PrevMainState;
	
	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	EMainState MainState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	ELocomotionState PostStopLocomotionState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData")
	float TimeInLocomotionState;

	UPROPERTY(BlueprintReadOnly, Category="EssentialData|Lean")
	float LeanX;
	
	UPROPERTY(BlueprintReadOnly, Category="EssentialData|Lean")
	float LeanY;

	UPROPERTY()
	FVector Lean;

	UPROPERTY(BlueprintReadOnly, Category="AimOffset")
	float AimPitch;
	
	UPROPERTY(BlueprintReadOnly, Category="AimOffset")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bPlayStartAnim;

	UPROPERTY(BlueprintReadOnly, Category="Locomotion")
	bool bPlayGaitTransitionAnim;

	UPROPERTY(BlueprintReadOnly, Category="Stride")
	float PlayRate;
	
	UPROPERTY(BlueprintReadOnly, Category="Stride")
	bool bJumpFootIsRight;

#pragma region ROTATION
	UPROPERTY(BlueprintReadOnly, Category="Rotation")
	float StartAngle;

	UPROPERTY(BlueprintReadOnly, Category="Rotation")
	FRotator StartRotation;

	UPROPERTY(BlueprintReadOnly, Category="Rotation")
	FRotator PrimaryTargetRotation;

	UPROPERTY(BlueprintReadOnly, Category="Rotation")
	FRotator SecondaryTargetRotation;
#pragma endregion

#pragma region ANIMATION_DATA

protected:
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* StopAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* CrouchStartAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* WalkStartAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* JogStartAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* TransitionToCrouchAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* TransitionToWalkAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* TransitionToJogAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* StartJumpAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* FallingAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* LandAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	UAnimSequence* InAirAnim;

	UPROPERTY(BlueprintReadOnly, Category="AnimationData")
	float AnimStartTime;
#pragma endregion

#pragma endregion

#pragma region SETTINGS

protected:
	UPROPERTY(EditDefaultsOnly, Category="Rotation")
	bool bDoInputRotation = false;

	UPROPERTY(EditDefaultsOnly, Category="Rotation")
	float PrimaryRotationInterpSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Rotation")
	float SecondaryRotationInterpSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Rotation")
	float AimYawLimit = 130.f;

	UPROPERTY(EditDefaultsOnly, Category="EssentialData")
	float LeanInterpSpeed = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion")
	float MinTimeInLocomotionState = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Start")
	float MaxSpeedForPlayingStartWalkAnim = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Start")
	float MaxSpeedForPlayingStartJogAnim = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Stop")
	float JogStopSpeedLimit = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Curves|Locomotion")
	FName MoveDataRotationName = "MoveData_Rotation";
	
	UPROPERTY(EditDefaultsOnly, Category="Curves|Locomotion")
	FName MoveDataRotationActiveName = "MoveData_RotationActive";

	UPROPERTY(EditDefaultsOnly, Category="Curves|Locomotion")
	FName MoveDataSpeedName = "MoveData_Speed";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Curves|Transition")
	FName TransitionIdleToCrouchCurveName = "TransitionData_IdleToCrouch";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Curves|Transition")
	FName TransitionWalkToCrouchCurveName = "TransitionData_WalkToCrouch";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Curves|Transition")
	FName TransitionJogToCrouchCurveName = "TransitionData_JogToCrouch";

	UPROPERTY(EditDefaultsOnly, Category="Curves|Lean")
	FName MoveDataLeanXName = "MoveData_LeanX";
	
	UPROPERTY(EditDefaultsOnly, Category="Curves|Lean")
	FName MoveDataLeanYName = "MoveData_LeanY";

	UPROPERTY(EditDefaultsOnly, Category="Bones")
	FName RootBoneName = "Root";
	
	UPROPERTY(EditDefaultsOnly, Category="Bones")
	FName RFootBoneName = "Foot_R";
	
	
#pragma region ANIMATIONS

#pragma region CROUCH_START
	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	UAnimSequence* CrouchStart90LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	float CrouchStart90LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	UAnimSequence* CrouchStart180LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	float CrouchStart180LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	UAnimSequence* CrouchStart90RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	float CrouchStart90RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	UAnimSequence* CrouchStart180RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	float CrouchStart180RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	UAnimSequence* CrouchStartFAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Crouch")
	float CrouchStartFAnimTime = 0.f;
#pragma endregion

#pragma region WALK_START
	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	UAnimSequence* WalkStart90LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	float WalkStart90LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	UAnimSequence* WalkStart180LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	float WalkStart180LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	UAnimSequence* WalkStart90RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	float WalkStart90RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	UAnimSequence* WalkStart180RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	float WalkStart180RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	UAnimSequence* WalkStartFAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Walk")
	float WalkStartFAnimTime = 0.f;
#pragma endregion

#pragma region JOG_START
	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	UAnimSequence* JogStart90LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	float JogStart90LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	UAnimSequence* JogStart180LAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	float JogStart180LAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	UAnimSequence* JogStart90RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	float JogStart90RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	UAnimSequence* JogStart180RAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	float JogStart180RAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	UAnimSequence* JogStartFAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Start|Jog")
	float JogStartFAnimTime = 0.f;
#pragma endregion

#pragma region STOP
	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	UAnimSequence* CrouchStopAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	float CrouchAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	UAnimSequence* WalkStopAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	float WalkAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	UAnimSequence* JogStopAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Stop")
	float JogAnimStartTime = 0.f;
#pragma endregion

#pragma region TRANSITION
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToWalk")
	UAnimSequence* CrouchToWalkAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToWalk")
	float CrouchToWalkAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToWalk")
	UAnimSequence* JogToWalkAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToWalk")
	float JogToWalkAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToCrouch")
	UAnimSequence* WalkToCrouchAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToCrouch")
	float WalkToCrouchAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToCrouch")
	UAnimSequence* JogToCrouchAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToCrouch")
	float JogToCrouchAnimStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToJog")
	UAnimSequence* CrouchToJogAnim;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToJog")
	float CrouchToJogAnimStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToJog")
	UAnimSequence* WalkToJogAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Transtion|ToJog")
	float WalkToJogAnimStartTime = 0.f;
#pragma endregion


#pragma region TRANSITION
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	UAnimSequence* IdleStartJumpAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	float IdleStartJumpStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	UAnimSequence* WalkStartJumpRAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	float WalkStartJumpRStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	UAnimSequence* WalkStartJumpLAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	float WalkStartJumpLStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	UAnimSequence* JogStartJumpRAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	float JogStartJumpRStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	UAnimSequence* JogStartJumpLAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Start")
	float JogStartJumpLStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Falling")
	UAnimSequence* IdleFallingAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Falling")
	UAnimSequence* WalkRFallingAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Falling")
	UAnimSequence* WalkLFallingAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Falling")
	UAnimSequence* JogRFallingAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Falling")
	UAnimSequence* JogLFallingAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	UAnimSequence* IdleLandAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	float IdleLandLStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	UAnimSequence* WalkRLandAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	float WalkRLandStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	UAnimSequence* WalkLLandAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	float WalkLLandStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	UAnimSequence* JogRLandAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	float JogRLandStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	UAnimSequence* JogLLandAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Animations|Jump|Land")
	float JogLLandStartTime = 0.f;
#pragma endregion 
	
#pragma endregion

#pragma region CROUCH
	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Crouch")
	float CrouchMinCurrentSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Crouch")
	float CrouchMinMaxSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Crouch")
	float CrouchMinInputAcceleration = 0.01f;
#pragma endregion

#pragma region WALK
	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Walk")
	float WalkMinCurrentSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Walk")
	float WalkMinMaxSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Walk")
	float WalkMinInputAcceleration = 0.01f;
#pragma endregion

#pragma region RUN
	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Jog")
	float JogMinCurrentSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Jog")
	float JogMinMaxSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Jog")
	float JogMinInputAcceleration = 0.5f;
#pragma endregion
#pragma endregion

#pragma region LOCOMOTION_STATE_CALLBACKS

private:
	OnEntryFlags_LocomotionState EntryFlags_LocomotionState;

#pragma region IDLE
	void OnEntryIdle();
	void OnExitIdle();
	void WhileTrueIdle();
	void WhileFalseIdle();
#pragma endregion

#pragma region WALK
	void OnEntryWalk();
	void OnExitWalk();
	void WhileTrueWalk();
	void WhileFalseWalk();
#pragma endregion

#pragma region JOG
	void OnEntryJog();
	void OnExitJog();
	void WhileTrueJog();
	void WhileFalseJog();
#pragma endregion

#pragma region CROUCH
	void OnEntryCrouch();
	void OnExitCrouch();
	void WhileTrueCrouch();
	void WhileFalseCrouch();
#pragma endregion
#pragma endregion

#pragma region MAIN_STATE_CALLBACKS
private:
	OnEntryFlags_MainState EntryFlags_MainState;
	
#pragma region ON_GROUND
	void OnEntryGroundState();
	void OnExitGroundState();
	void WhileTrueGroundState();
	void WhileFalseGroundState();
#pragma endregion

#pragma region ON_LADDER
	void OnEntryLadderState();
	void OnExitLadderState();
	void WhileTrueLadderState();
	void WhileFalseLadderState();
#pragma endregion

#pragma region ON_AIR
	void OnEntryAirState();
	void OnExitAirState();
	void WhileTrueAirState();
	void WhileFalseAirState();
#pragma endregion
	
#pragma endregion 
	
#pragma region ON_LADDER
public:
	void UpdateEnterSideLadder();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Ladder|AnimationData")
	UAnimSequence* EnterLadderAnim;
	
	UPROPERTY(BlueprintReadOnly, Category="Ladder|AnimationData")
	UAnimSequence* ExitLadderAnim;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Ladder|Animations|Enter")
	UAnimSequence* LadderEnterTopAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Ladder|Animations|Enter")
	float LadderEnterTopStartTime;
	
	UPROPERTY(EditDefaultsOnly, Category="Ladder|Animations|Enter")
	UAnimSequence* LadderEnterBottomAnim;
	
	UPROPERTY(EditDefaultsOnly, Category="Ladder|Animations|Enter")
	float LadderEnterBottomStartTime;
#pragma endregion 
	
	
#pragma region HELPERS

private:
	FORCEINLINE bool IsMovementWithinThresholds(float MinCurrentSpeed, float MinMaxSpeed,
	                                            float MinInputAcceleration) const;

	FORCEINLINE void TrackLocomotionState(ELocomotionState TracedState,
	                                      bool& EnterFlag,
	                                      void (UPlayerAnimInstance::*OnEnterCallback)(),
	                                      void (UPlayerAnimInstance::*OnExitCallback)(),
	                                      void (UPlayerAnimInstance::*WhileTrueCallback)(),
	                                      void (UPlayerAnimInstance::*WhileFalseCallback)());

	FORCEINLINE void TrackMainState(EMainState TracedState,
										  bool& EnterFlag,
										  void (UPlayerAnimInstance::*OnEnterCallback)(),
										  void (UPlayerAnimInstance::*OnExitCallback)(),
										  void (UPlayerAnimInstance::*WhileTrueCallback)(),
										  void (UPlayerAnimInstance::*WhileFalseCallback)());

	FORCEINLINE void UpdateStartAnim(UAnimSequence*& FinishAnim,
	                                 UAnimSequence* Start90LAnim, float Start90LAnimTime,
	                                 UAnimSequence* Start180LAnim, float Start180LAnimTime,
	                                 UAnimSequence* Start90RAnim, float Start90RAnimTime,
	                                 UAnimSequence* Start180RAnim, float Start180RAnimTime,
	                                 UAnimSequence* StartFAnim, float StartFAnimTime);

#pragma endregion
};
