// OutbreakZ: Mobile. All rights reserved.


#include "Vehicle/SurvivalVehiclePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASurvivalVehiclePawn::ASurvivalVehiclePawn()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(GetMesh());
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, SpringArm->SocketName);
}

void ASurvivalVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	if(const auto VehicleMovement = GetVehicleMovementComponent())
	{
		ChaosWheeledVehicleMovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(VehicleMovement);
	}
}

void ASurvivalVehiclePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateMeshDamping();
	InterpsToOriginalRotation();
}

void ASurvivalVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}
}

void ASurvivalVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASurvivalVehiclePawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASurvivalVehiclePawn::Throttle);
		
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ASurvivalVehiclePawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ASurvivalVehiclePawn::Steering);

		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASurvivalVehiclePawn::BrakeTriggered);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ASurvivalVehiclePawn::BrakeStarted);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASurvivalVehiclePawn::BrakeCompleted);

		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &ASurvivalVehiclePawn::HandbrakeTriggered);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ASurvivalVehiclePawn::HandbrakeCompleted);
		
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASurvivalVehiclePawn::LookAround);
	}
}

void ASurvivalVehiclePawn::Throttle(const FInputActionValue& Value)
{
	const auto Axis = Value.Get<float>();
	ChaosWheeledVehicleMovementComponent->SetThrottleInput(Axis);
}

void ASurvivalVehiclePawn::Steering(const FInputActionValue& Value)
{
	const auto Axis = Value.Get<float>();
	ChaosWheeledVehicleMovementComponent->SetSteeringInput(Axis);
}

void ASurvivalVehiclePawn::BrakeTriggered(const FInputActionValue& Value)
{
	const auto Axis = Value.Get<float>();
	ChaosWheeledVehicleMovementComponent->SetBrakeInput(Axis);
}

void ASurvivalVehiclePawn::BrakeStarted(const FInputActionValue& Value)
{
	BrakeLights(true);
}

void ASurvivalVehiclePawn::BrakeCompleted(const FInputActionValue& Value)
{
	const auto Axis = Value.Get<float>();
	ChaosWheeledVehicleMovementComponent->SetBrakeInput(Axis);
	
	BrakeLights(false);
}

void ASurvivalVehiclePawn::HandbrakeTriggered(const FInputActionValue& Value)
{
	ChaosWheeledVehicleMovementComponent->SetHandbrakeInput(true);
	BrakeLights(true);
}

void ASurvivalVehiclePawn::HandbrakeCompleted(const FInputActionValue& Value)
{
	ChaosWheeledVehicleMovementComponent->SetHandbrakeInput(false);
	BrakeLights(false);
}

void ASurvivalVehiclePawn::LookAround(const FInputActionValue& Value)
{
	const auto Axis = Value.Get<float>();
	SpringArm->AddLocalRotation(FRotator(0.f, Axis, 0.f));
}

void ASurvivalVehiclePawn::UpdateMeshDamping()
{
	float InDamping = 3.0;
	if(ChaosWheeledVehicleMovementComponent->IsMovingOnGround())
	{
		InDamping = 0.f;
	}

	GetMesh()->SetAngularDamping(InDamping);
}

void ASurvivalVehiclePawn::InterpsToOriginalRotation()
{
	const auto CurrentYaw = SpringArm->GetRelativeRotation().Yaw;
	const auto DeltaSeconds = GetWorld()->GetDeltaSeconds();
	
	const auto NewRotationYaw = UKismetMathLibrary::FInterpTo(CurrentYaw, 0.f, DeltaSeconds, InterpCameraRotationSpeed);
	SpringArm->SetRelativeRotation(FRotator(0.f, NewRotationYaw, 0.f));
}
