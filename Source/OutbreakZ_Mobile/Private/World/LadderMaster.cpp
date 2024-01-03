// OutbreakZ: Mobile. All rights reserved.


#include "World/LadderMaster.h"

#include "Components/ArrowComponent.h"

ALadderMaster::ALadderMaster()
{
 	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
}

void ALadderMaster::BeginPlay()
{
	Super::BeginPlay();
	
}

