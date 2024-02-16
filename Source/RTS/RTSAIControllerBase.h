// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTSAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ARTSAIControllerBase : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree = nullptr;
	
};
