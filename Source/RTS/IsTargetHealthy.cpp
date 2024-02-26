// Fill out your copyright notice in the Description page of Project Settings.


#include "IsTargetHealthy.h"
#include "UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSAIControllerBase.h"

UIsTargetHealthy::UIsTargetHealthy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

bool UIsTargetHealthy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	//Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();
	ARTSAIControllerBase* OwnerAIController = Cast<ARTSAIControllerBase>(AIController);
	AUnitBase* OwnerUnit = Cast<AUnitBase>(AIController->GetPawn());
	AUnitBase* TargetUnit = Cast<AUnitBase>(OwnerUnit->TargetActor);

	// No controller means that target is dead and unit should stop attacking it
	if (TargetUnit->bIsAlive == true && TargetUnit->UnitCurrentHealth >= TargetUnit->UnitMaxHealth)
	{
		OwnerAIController->ChooseNewTarget();
		return true;
	}
	else
	{
		return false;
	}

}
