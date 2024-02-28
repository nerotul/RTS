// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsTargetHealthy.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API UIsTargetHealthy : public UBTDecorator
{
	GENERATED_BODY()

public:
	UIsTargetHealthy(const FObjectInitializer& ObjInit);

protected:
	// Perform Condition Check AI (Blueprint)
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
