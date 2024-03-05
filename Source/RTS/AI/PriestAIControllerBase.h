// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSAIControllerBase.h"
#include "PriestAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API APriestAIControllerBase : public ARTSAIControllerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Looks for an ally with wounds
	virtual void EnemySensed(AActor* SensedActor, FAIStimulus Stimulus) override;

public:
	virtual void ChooseNewTarget() override;

	void CheckIfEnemiesPerceived();
	
};
