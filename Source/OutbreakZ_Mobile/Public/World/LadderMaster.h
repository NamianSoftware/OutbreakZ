// OutbreakZ: Mobile. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadderMaster.generated.h"

class UArrowComponent;

UCLASS()
class OUTBREAKZ_MOBILE_API ALadderMaster : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LadderSettings")
	float LadderHeight = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LadderSettings")
	float LadderGap = 35.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LadderSettings")
	float LadderBarsOffset = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LadderSettings")
	bool bHasScrews = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LadderSettings", meta = (EditCondition = "bHasScrews"))
	int32 ScrewsInterval = 3.f;
	
	
public:	
	ALadderMaster();

protected:
	virtual void BeginPlay() override;

};
