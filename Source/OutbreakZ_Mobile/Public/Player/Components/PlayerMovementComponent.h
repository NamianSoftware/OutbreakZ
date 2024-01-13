// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct FMovementStateConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float MaxWalkSpeed = 165.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxAcceleration = 300.f;
	
	UPROPERTY(EditDefaultsOnly)
	float BrakingDeceleration = 300.f;
};

UENUM(BlueprintType)
enum class EAppliedMovementState : uint8
{
	EAMS_None,
	EAMS_Walk,
	EAMS_Jog,
	EAMS_Run
};


UCLASS()
class OUTBREAKZ_MOBILE_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_CMP : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToJog:1;

	public:
		FSavedMove_CMP();
		
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_CMP : public FNetworkPredictionData_Client_Character
	{	
		typedef FNetworkPredictionData_Client_Character Super;
		
	public:
		FNetworkPredictionData_Client_CMP(const UCharacterMovementComponent& ClientMovement);
		
		virtual FSavedMovePtr AllocateNewMove() override;
	};

	bool Safe_bWantsToJog;

	UPROPERTY(EditDefaultsOnly, Category=MovementConfig) FMovementStateConfig WalkConfig;
	UPROPERTY(EditDefaultsOnly, Category=MovementConfig) FMovementStateConfig JogConfig;

	UPROPERTY() EAppliedMovementState AppliedMovementState = EAppliedMovementState::EAMS_None;

public:
	UPlayerMovementComponent();
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	UFUNCTION(BlueprintCallable) void StartJog();
	UFUNCTION(BlueprintCallable) void StopJog();
	
	UFUNCTION(BlueprintCallable) void ToggleCrouch();
	UFUNCTION(BlueprintCallable) void StartCrouch();
	UFUNCTION(BlueprintCallable) void StopCrouch();
	
	UFUNCTION(BlueprintCallable) void ClimbToLadder();
	UFUNCTION(BlueprintCallable) void UnclimbLadder();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsOnLadder() const { return bIsOnLadder; }
	
private:	
	UPROPERTY()
	bool bIsOnLadder = false;
	
private:
	void ApplyMovementConfig(const FMovementStateConfig& MovementConfig);
};
