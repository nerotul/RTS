// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsTargetAvailable.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API UIsTargetAvailable : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UIsTargetAvailable(const FObjectInitializer& ObjInit);

protected:
	// Perform Condition Check AI (Blueprint)
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// Receive Execution Start (Blueprint)
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	// Receive Execution Ended (Blueprint)
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	// Called when underlying node was processed (deactivated or failed to activate) this function should be considered as const (don't modify state of object) if node is not instanced!
	virtual void OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) override;
	// Receive Activation (Blueprint)
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// Receive Deactivation (Blueprint)
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// Receive Tick (Blueprint)
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
