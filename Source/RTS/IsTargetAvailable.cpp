// Fill out your copyright notice in the Description page of Project Settings.


#include "IsTargetAvailable.h"
#include "AIController.h"
#include "UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "RTSAIControllerBase.h"

UIsTargetAvailable::UIsTargetAvailable(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

bool UIsTargetAvailable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	//Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();
	ARTSAIControllerBase* OwnerAIController = Cast<ARTSAIControllerBase>(AIController);
	AUnitBase* OwnerUnit = Cast<AUnitBase>(AIController->GetPawn());
	AUnitBase* TargetUnit = Cast<AUnitBase>(OwnerUnit->TargetActor);

	// No controller means that target is dead and unit should stop attacking it
	if (TargetUnit->bIsAlive == true)
	{
		return true;
	}
	else
	{	
		OwnerAIController->ChooseNewTarget();
		return false;
	}
}

void UIsTargetAvailable::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
}

void UIsTargetAvailable::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	Super::OnNodeDeactivation(SearchData, NodeResult);
}

void UIsTargetAvailable::OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult)
{
	Super::OnNodeProcessed(SearchData, NodeResult);
}

void UIsTargetAvailable::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UIsTargetAvailable::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UIsTargetAvailable::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
