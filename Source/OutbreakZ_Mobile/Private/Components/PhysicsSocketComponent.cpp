// OutbreakZ: Mobile. All rights reserved.


#include "Components/PhysicsSocketComponent.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

UPhysicsSocketComponent::UPhysicsSocketComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SocketRoot = CreateDefaultSubobject<UStaticMeshComponent>("SocketRoot");
	SocketRoot->SetVisibility(false);
	SocketRoot->SetHiddenInGame(true);

	SocketHinge = CreateDefaultSubobject<UStaticMeshComponent>("SocketHinge");
	SocketHinge->SetVisibility(false);
	SocketHinge->SetHiddenInGame(true);
	SocketHinge->SetSimulatePhysics(true);
	SocketHinge->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SocketHinge->SetCollisionObjectType(ECC_PhysicsBody);
	SocketHinge->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	SocketPhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("SocketPhysicsConstraint");
	SocketPhysicsConstraint->SetConstrainedComponents(SocketRoot, NAME_None, SocketHinge, NAME_None);
	
}


void UPhysicsSocketComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPhysicsSocketComponent::Attach(USceneComponent* InParent, FName InSocketName)
{
	SetupAttachment(InParent, InSocketName);
	
	SocketRoot->SetupAttachment(this);
	SocketHinge->SetupAttachment(this);
	SocketPhysicsConstraint->SetupAttachment(SocketRoot);
}
