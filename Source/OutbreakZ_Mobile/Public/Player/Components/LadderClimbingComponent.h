// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LadderClimbingComponent.generated.h"


class ALadderMaster;
class ASurvivalCharacter;
class UPlayerMovementComponent;
class UMotionWarpingComponent;


UENUM(BlueprintType)
enum class ELadderEnterSide : uint8
{
	ELES_FromTop UMETA(DisplayName = "FromTop"),
	ELES_FromBottom UMETA(DisplayName = "FromBottom"),
	// ELES_FromCenter UMETA(DisplayName = "FromCenter"),
};


UENUM(BlueprintType)
enum class ELadderMoveDirection : uint8
{
	ELMD_None UMETA(DisplayName = "None"),
	ELMD_MoveUp UMETA(DisplayName = "MoveUp"),
	ELMD_MoveDown UMETA(DisplayName = "MoveDown"),
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OUTBREAKZ_MOBILE_API ULadderClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULadderClimbingComponent();

protected:
	virtual void BeginPlay() override;

public:
	void LadderInput(float Axis);

	bool FindLadder();
	bool OnLadderCheck();
	void Unclimb();

	UFUNCTION(BlueprintPure)
	FORCEINLINE ELadderEnterSide GetLadderEnterSide() const { return LadderEnterSide; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ELadderMoveDirection GetLadderMoveDirection() const { return LadderMoveDirection; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ALadderMaster* GetLadderRef() const { return LadderRef; }
	
	void SetMovementComponentRef(UPlayerMovementComponent* ComponentRef) { PlayerMovementComponent = ComponentRef; }

private:
	UPROPERTY()
	ASurvivalCharacter* PlayerRef;

	UPROPERTY()
	UPlayerMovementComponent* PlayerMovementComponent;

	UPROPERTY()
	ALadderMaster* LadderRef;

	ELadderEnterSide LadderEnterSide;
	ELadderMoveDirection LadderMoveDirection;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float TraceRadius = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float MoveToOverTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings|Targets")
	FName EnterLadderName = "EnterLadder";
};
