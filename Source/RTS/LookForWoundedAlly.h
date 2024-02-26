// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "LookForWoundedAlly.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ULookForWoundedAlly : public UBTService
{
	GENERATED_BODY()
	
public:
	ULookForWoundedAlly(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
