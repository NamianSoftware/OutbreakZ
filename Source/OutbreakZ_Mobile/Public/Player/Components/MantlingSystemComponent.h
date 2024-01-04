// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MantlingSystemComponent.generated.h"


class AActor;
class ACharacter;
class UCharacterMovementComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class UAnimInstance;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMantleStateUpdate, bool, bIsMantling);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OUTBREAKZ_MOBILE_API UMantlingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling System")
	bool bIsEnabled;

	/**
	*Actors to be ignored by Line Trace
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace")
	TArray<AActor*> ActorsToIgnore;
	/**
	*Where the Line Trace Should be drawn, beginning from the Actor Location
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace")
	float FwdOffset;
	/**
	*Line Trace Begin Height, beginning from the top of the Character, i.e. (ActorLoc.Z + CapsuleHalfHegith)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace")
	float HighOffset;
	/**
	*Line Trace End Height, beginning from the bottom of the Character, i.e. (ActorLoc.Z - CapsuleHalfHegith)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace")
	float LowOffset;

	UPROPERTY(EditAnywhere, Category = "Line Trace", meta = (DisplayName = "Pelvis Socket Name"))
	FName Pelvis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")
	float ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* Montage_High;
	/**
	*Time at Montage Starts to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	float Start_High;
	UPROPERTY(EditAnywhere, Category = "Montage")
	float NotifyBegin_High;
	UPROPERTY(EditAnywhere, Category = "Montage")
	float NotifyDuration_High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* Montage_Low;
	/**
	*Time at Montage Starts to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	float Start_Low;
	UPROPERTY(EditAnywhere, Category = "Montage")
	float NotifyBegin_Low;
	UPROPERTY(EditAnywhere, Category = "Montage")
	float NotifyDuration_Low;

	/**
	*Distance to Location, where Character should begin grabbing. Beginning from top of Actor, i. e.
	*ActorLocation.Z + CapsuleHalfHeight. In Z Axis.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	float DistanceToGrab;
	/**
	*How close do we have to be to the Grabbing Point to begin Mantle
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	float MantleEnterThreshold;

	//float MantleFwdAmount;
	/**
	*Amount each frame Actor Location changes during mantling. In the direction of Forward Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float MantleFwdAmount_High;
	/**
	*Amount each frame Actor Location changes during mantling. In the direction of Forward Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float MantleFwdAmount_Low;

	bool bHighMantle;

	//float MantleUpAmount;
	/**
	*Amount each frame Actor Location changes during mantling. In the direction of Up Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float MantleUpAmount_High;
	/**
	*Amount each frame Actor Location changes during mantling. In the direction of Up Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float MantleUpAmount_Low;

	UPROPERTY(BlueprintReadOnly, Category = "Mantling")
	bool bIsMantling;
	UPROPERTY(BlueprintReadOnly, Category = "Mantling")
	bool bHit;

	/**
	*Offset when adjusting Actor Location in the last frame of mantling, Up Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float EndOffset_Up;
	/**
	*Offset when adjusting Actor Location in the last frame of mantling, Fwd Vector
	*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mantling")
	//float EndOffset_Fwd;

	/* DEBUG */
	UPROPERTY(EditDefaultsOnly, Category="Mantling|Debug")
	bool bIsDebug = true;

	/* Delegates */
	FOnMantleStateUpdate OnMantleStateUpdate;

	UMantlingSystemComponent();

protected:

	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multi_Mantle(AActor* InHitActor, FVector InLocation, FRotator InRotation, FVector InHitLocation, UAnimMontage* MantleMontage);
	virtual void Multi_Mantle_Implementation(AActor* InHitActor, FVector InLocation, FRotator InRotation, FVector InHitLocation,
		UAnimMontage* MantleMontage);

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	float CapsuleHH;

	bool bDoOnce_MantleEnter;

	FVector HitLoc;

	FVector MantleBegin_ActorLoc;
	FVector Mantle_ActorLoc;
	FRotator Mantle_RootRot;

	AActor* HitActor;
	bool bSetActorLoc;

	int32 MontageInstanceID;

	//void UnbindDelegates();

	AActor* Actor;
	ACharacter* Char;
	UCharacterMovementComponent* CharMov;
	UCapsuleComponent* Capsule;
	USkeletalMeshComponent* Mesh;
	UAnimInstance* AnimInstance;

	void LineTrace(FHitResult& HitResult, FVector Start, FVector End);
	//float GetMontagePos(UAnimMontage* Montage);

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
