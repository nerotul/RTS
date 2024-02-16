// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAIControllerBase.h"

void ARTSAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree != nullptr)
	{
		RunBehaviorTree(BehaviorTree);
	}
}
