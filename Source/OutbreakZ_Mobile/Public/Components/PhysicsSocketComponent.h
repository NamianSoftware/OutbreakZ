// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PhysicsSocketComponent.generated.h"


class UPhysicsConstraintComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OUTBREAKZ_MOBILE_API UPhysicsSocketComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* SocketRoot;
	
	UPROPERTY(EditAnywhere, Category="Components")
	UPhysicsConstraintComponent* SocketPhysicsConstraint;
	
	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* SocketHinge;
	
public:	
	UPhysicsSocketComponent();

protected:
	virtual void BeginPlay() override;

public:
	void Attach(USceneComponent* InParent, FName InSocketName = NAME_None);
		
};
