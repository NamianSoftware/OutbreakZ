// OutbreakZ: Mobile. All rights reserved.


#include "Player/SurvivalCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASurvivalCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
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

