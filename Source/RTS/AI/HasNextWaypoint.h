// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "HasNextWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API UHasNextWaypoint : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UHasNextWaypoint(const FObjectInitializer& ObjInit);

protected:
	// Perform Condition Check AI (Blueprint)
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


};
