// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "WheeledVehiclePawn.h"
#include "SurvivalVehiclePawn.generated.h"


class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UChaosWheeledVehicleMovementComponent;


UCLASS()
class OUTBREAKZ_MOBILE_API ASurvivalVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category=Camera)
	float InterpCameraRotationSpeed = 1.f;

public:
	ASurvivalVehiclePawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


#pragma region BLUEPRINT_IMPLEMENTED
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BrakeLights(bool IsBraking);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float GetSteeringAngle();
	
#pragma endregion

#pragma region INPUT

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* VehicleMappingContext;

	/** Throttle Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrottleAction;

	/** Brake Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BrakeAction;

	/** Handbrake Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HandbrakeAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAroundAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SteeringAction;

protected:
	/** Called for throttle input */
	void Throttle(const FInputActionValue& Value);

	/** Called for steering input */
	void Steering(const FInputActionValue& Value);

	/** Called for brake input */
	void BrakeTriggered(const FInputActionValue& Value);
	void BrakeStarted(const FInputActionValue& Value);
	void BrakeCompleted(const FInputActionValue& Value);
	
	/** Called for handbrake input */
	void HandbrakeTriggered(const FInputActionValue& Value);
	void HandbrakeCompleted(const FInputActionValue& Value);
	
	/** Called for looking input */
	void LookAround(const FInputActionValue& Value);

#pragma endregion

protected:
	UPROPERTY(BlueprintReadOnly)
	UChaosWheeledVehicleMovementComponent* ChaosWheeledVehicleMovementComponent;

private:
	void UpdateMeshDamping();
	void InterpsToOriginalRotation();

};
