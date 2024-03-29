// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "RTS/Units/UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"

ARTSAIControllerBase::ARTSAIControllerBase(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	AIPerceptionComponent->ConfigureSense(*DamageSenseConfig);

	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->PeripheralVisionAngleDegrees = 180.0f;
	SightSenseConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightSenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightSenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComponent->ConfigureSense(*SightSenseConfig);
}

void ARTSAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree != nullptr)
	{
		RunBehaviorTree(BehaviorTree);
	}

	ControlledUnit = Cast<AUnitBase>(GetPawn());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ARTSAIControllerBase::EnemySensed);
	UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);

}

void ARTSAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (IsValid(ControlledUnit))
	{
		// If unit has no target and sees enemy, it attacks this enemy
		if (Stimulus.WasSuccessfullySensed() == true && CheckIfEnemyUnitAndAlive(SensedUnit) && ControlledUnit->TargetActor == nullptr)
		{
			ControlledUnit->SetAttackTargetActor(SensedUnit);

			if (ControlledUnit->bIsPlayersUnit == false && ControlledUnit->bIsAlive == true)
			{
				ControlledUnit->SetUnitVisibility(true);
				ControlledUnit->OnInEnemySight();
			}

		}
		// If unit has lost target from sight, it tries to find another target
		else if (Stimulus.WasSuccessfullySensed() == false && CheckIfEnemyUnitAndAlive(SensedUnit))
		{
			ChooseNewTarget();
		}

	}
}

void ARTSAIControllerBase::FinishRepositionUnit()
{
	if (IsValid(UnitBlackboard))
	{
		UnitBlackboard->SetValueAsBool(FName("bIsRepositioning"), false);
	}
}

AUnitBase* ARTSAIControllerBase::FindClosestEnemyInSight(const TArray<AActor*> InPerceivedActors)
{
	float MinDistance = 99999.0f;
	AUnitBase* ClosestEnemy = nullptr;

	if (IsValid(ControlledUnit))
	{
		for (AActor* Actor : PerceivedActors)
		{
			AUnitBase* Unit = Cast<AUnitBase>(Actor);
			if (IsValid(Unit) && CheckIfEnemyUnitAndAlive(Unit) && ControlledUnit->GetDistanceTo(Unit) < MinDistance)
			{
				MinDistance = ControlledUnit->GetDistanceTo(Unit);
				ClosestEnemy = Unit;
			}
		}

	}

	PerceivedActors.Empty();

	return ClosestEnemy;
}

bool ARTSAIControllerBase::CheckIfEnemyUnitAndAlive(const AUnitBase* InEnemyUnit)
{
	if (IsValid(InEnemyUnit) && InEnemyUnit->bIsAlive == true && InEnemyUnit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit)
	{
		return true;
	}

	return false;
}

void ARTSAIControllerBase::RepositionUnit()
{
	if (IsValid(UnitBlackboard))
	{
		UnitBlackboard->SetValueAsBool(FName("bIsRepositioning"), true);
		GetWorldTimerManager().SetTimer(SightCooloffTimer, this, &ARTSAIControllerBase::FinishRepositionUnit, 1.0f, false, 0.5f);

	}
}

void ARTSAIControllerBase::StopUnitMovement()
{
	if (IsValid(UnitBlackboard) && IsValid(ControlledUnit))
	{
		UnitBlackboard->SetValueAsVector(FName("TargetLocation"), ControlledUnit->GetActorLocation());
		MovementWaypoints.Empty();
	}
}

void ARTSAIControllerBase::ChooseNewTarget()
{
	if (IsValid(ControlledUnit))
	{
		StopUnitMovement();

		AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);
		AUnitBase* ClosestEnemy = FindClosestEnemyInSight(PerceivedActors);
		ControlledUnit->SetAttackTargetActor(ClosestEnemy);

		if (!IsValid(ClosestEnemy) && ControlledUnit->bIsPlayersUnit == false && ControlledUnit->bIsAlive == true)
		{
			ControlledUnit->SetUnitVisibility(false);
		}

	}
}

