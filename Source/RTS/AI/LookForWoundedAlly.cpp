// Fill out your copyright notice in the Description page of Project Settings.


#include "LookForWoundedAlly.h"
#include "RTS/AI/RTSAIControllerBase.h"


ULookForWoundedAlly::ULookForWoundedAlly(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "My Service";

	bNotifyBecomeRelevant = true;

	Interval = 0.5f;
	RandomDeviation = 0.f;
}

void ULookForWoundedAlly::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	ARTSAIControllerBase* OwnerAIController = Cast<ARTSAIControllerBase>(AIController);
	OwnerAIController->ChooseNewTarget();

}