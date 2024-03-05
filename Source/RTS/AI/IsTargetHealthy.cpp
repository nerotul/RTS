// Fill out your copyright notice in the Description page of Project Settings.


#include "IsTargetHealthy.h"
#include "RTS/Units/UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSAIControllerBase.h"
#include "PriestAIControllerBase.h"
#include "RTS/GAS/RTSAttributeSet.h"

UIsTargetHealthy::UIsTargetHealthy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

bool UIsTargetHealthy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APriestAIControllerBase* OwnerAIController = Cast<APriestAIControllerBase>(AIController);
	AUnitBase* OwnerUnit = Cast<AUnitBase>(AIController->GetPawn());
	AUnitBase* TargetUnit = Cast<AUnitBase>(OwnerUnit->TargetActor);

	// This check is called here to avoid using tick in priest controller
	if (OwnerUnit->bIsPlayersUnit == false)
	{
		OwnerAIController->CheckIfEnemiesPerceived();
	}

	// Change target if current target unit has full health
	if (TargetUnit->bIsAlive == true && TargetUnit->AttributeSet->GetHealth() >= TargetUnit->AttributeSet->GetMaxHealth() || OwnerUnit->bIsPlayersUnit != TargetUnit->bIsPlayersUnit)
	{
		OwnerAIController->ChooseNewTarget();
		return true;
	}
	else
	{
		return false;
	}

}
