// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "RTSAIControllerBase.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;
class AUnitBase;

/**
 * 
 */
UCLASS()
class RTS_API ARTSAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	ARTSAIControllerBase();

	UBlackboardComponent* UnitBlackboard = nullptr;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAIPerceptionComponent* AIPerceptionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAISenseConfig_Damage* DamageSenseConfig = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAISenseConfig_Sight* SightSenseConfig = nullptr;

	AUnitBase* ControlledUnit = nullptr;

	TArray<AActor*> PerceivedActors;

	UFUNCTION()
	virtual void EnemySensed(AActor* SensedActor, FAIStimulus Stimulus);

	FTimerHandle SightCooloffTimer;

	void FinishRepositionUnit();

	AUnitBase* FindClosestEnemyInSight(const TArray<AActor*> InPerceivedActors);

	bool CheckIfEnemyUnitAndAlive(const AUnitBase* InEnemyUnit);

public:
	virtual void ChooseNewTarget();

	// Making unit to ignore enemies for some time while moving
	void RepositionUnit();
	
};
