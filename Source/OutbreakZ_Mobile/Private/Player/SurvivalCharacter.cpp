// OutbreakZ: Mobile. All rights reserved.


#include "Player/SurvivalCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BaseGizmos/GizmoElementShared.h"
#include "Player/Components/PlayerMovementComponent.h"

ASurvivalCharacter::ASurvivalCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, SpringArm->SocketName);

	PlayerMovementComponent = Cast<UPlayerMovementComponent>(GetCharacterMovement());
}

void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Look);

		EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Started, this, &ASurvivalCharacter::JogStarted);
		EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Canceled, this, &ASurvivalCharacter::JogFinished);
		EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Completed, this, &ASurvivalCharacter::JogFinished);
	}
}

void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	MoveForward(MovementVector.Y);
	MoveRight(MovementVector.X);
}

void ASurvivalCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	LookUp(LookAxisVector.Y);
	LookRight(LookAxisVector.X);
}

void ASurvivalCharacter::JogStarted(const FInputActionValue& Value)
{
	if (!Controller) return;

	PlayerMovementComponent->StartJog();
}

void ASurvivalCharacter::JogFinished(const FInputActionValue& Value)
{
	if (!Controller) return;

	PlayerMovementComponent->StopJog();
}

void ASurvivalCharacter::MoveForward(float AxisValue)
{
	const auto ClampedAxisValue = ClampAxisValue(AxisValue);
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, ClampedAxisValue);
}

void ASurvivalCharacter::MoveRight(float AxisValue)
{
	const auto ClampedAxisValue = ClampAxisValue(AxisValue);
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, ClampedAxisValue);
}

void ASurvivalCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void ASurvivalCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

float ASurvivalCharacter::ClampAxisValue(float AxisValue) const
{
	if(FMath::Abs(AxisValue) < MobileDeadZone)
	{
		return 0.f;
	}
	
	return AxisValue > 0.f ? 1.f : -1.f;
}
