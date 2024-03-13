// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS/AI/HasNextWaypoint.h"
#include "AIController.h"
#include "RTS/Units/UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UHasNextWaypoint::UHasNextWaypoint(const FObjectInitializer& ObjInit)
{
}

bool UHasNextWaypoint::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ARTSAIControllerBase* OwnerAIController = Cast<ARTSAIControllerBase>(AIController);

	FVector LastUnitLocation = OwnerAIController->UnitBlackboard->GetValueAsVector(FName("TargetLocation"));
	int32 LastUnitLocationIndex = OwnerAIController->MovementWaypoints.Find(LastUnitLocation);

	if (OwnerAIController->MovementWaypoints.IsValidIndex(LastUnitLocationIndex+1))
	{
		OwnerAIController->UnitBlackboard->SetValueAsVector(FName("TargetLocation"), OwnerAIController->MovementWaypoints[LastUnitLocationIndex+1]);
		return true;
	}
	else
	{
		OwnerAIController->StopUnitMovement();
		OwnerAIController->MovementWaypoints.Empty();
		return false;
	}
}
