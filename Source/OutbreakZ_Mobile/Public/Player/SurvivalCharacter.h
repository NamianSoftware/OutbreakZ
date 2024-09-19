// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UPlayerMovementComponent;
class UPhysicsSocketComponent;
class ULadderClimbingComponent;
class UMantlingSystemComponent;
class UMotionWarpingComponent;


UCLASS()
class OUTBREAKZ_MOBILE_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivalCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region COMPONENTS

protected:
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	UPlayerMovementComponent* PlayerMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Meshes)
	USceneComponent* VisualPart;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Meshes)
	USkeletalMeshComponent* BodyMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Meshes)
	UStaticMeshComponent* HeadItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Meshes)
	FName HeadItemSocket = "HeadSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Meshes)
	UStaticMeshComponent* BackpackMesh;
#pragma endregion
	
	UPROPERTY(EditDefaultsOnly, Category="Components|Mantle")
	UMantlingSystemComponent* MantleComponent;

#pragma region PHYSICS_SOCKET
	UPROPERTY(EditDefaultsOnly, Category=PhysicsSockets)
	UPhysicsSocketComponent* BackpackSocket;

	UPROPERTY(EditDefaultsOnly, Category=PhysicsSockets)
	FName BackpackSocketName = "BackpackSocket";
#pragma endregion

#pragma region INPUT

private:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Jog Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JogAction;
	
	/** Jog Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Jog Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for jog input */
	void JogStarted(const FInputActionValue& Value);
	void JogFinished(const FInputActionValue& Value);

	/** Called for jog input */
	void CrouchToggled(const FInputActionValue& Value);
	void CrouchStarted(const FInputActionValue& Value);
	void CrouchFinished(const FInputActionValue& Value);

	void JumpPressed(const FInputActionValue& Value);

protected:
	UFUNCTION(BlueprintCallable)
	void MoveForward(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void LookUp(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void LookRight(float AxisValue);

	float ClampAxisValue(float AxisValue) const;

	UPROPERTY(EditDefaultsOnly, Category="Settings|MobileInput")
	float MobileDeadZone = 0.2;
#pragma endregion
};
