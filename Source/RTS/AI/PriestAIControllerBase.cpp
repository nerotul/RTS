// Fill out your copyright notice in the Description page of Project Settings.


#include "PriestAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "RTS/Units/UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS/GAS/RTSAttributeSet.h"

void APriestAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void APriestAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (Stimulus.WasSuccessfullySensed() == true && CheckIfAllyAndWounded(SensedUnit) && ControlledUnit->TargetActor == nullptr)
	{
		ControlledUnit->SetAttackTargetActor(SensedUnit);

	}
	else if (Stimulus.WasSuccessfullySensed() == false && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit)
	{
		ChooseNewTarget();
	}

	if (Stimulus.WasSuccessfullySensed() == true && CheckIfEnemyUnitAndAlive(SensedUnit))
	{
		if (ControlledUnit->bIsPlayersUnit == false && ControlledUnit->bIsAlive == true)
		{
			ControlledUnit->SetUnitVisibility(true);
			StopUnitMovement();
		}

	}
}

AUnitBase* APriestAIControllerBase::FindMostWoundedAllyInSight(const TArray<AActor*> InPerceivedActors)
{
	float MinHealth = 99999.0f;
	AUnitBase* ClosestAlly = nullptr;

	for (AActor* Actor : PerceivedActors)
	{
		AUnitBase* Unit = Cast<AUnitBase>(Actor);
		if (CheckIfAllyAndWounded(Unit) && Unit->AttributeSet->GetHealth() < MinHealth)
		{
			MinHealth = Unit->AttributeSet->GetHealth();
			ClosestAlly = Unit;
		}
	}

	PerceivedActors.Empty();

	return ClosestAlly;
}

bool APriestAIControllerBase::CheckIfAllyAndWounded(const AUnitBase* InAllyUnit)
{
	if (IsValid(InAllyUnit) && InAllyUnit->bIsAlive == true && InAllyUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit && 
		InAllyUnit->AttributeSet->GetHealth() < InAllyUnit->AttributeSet->GetMaxHealth())
	{
		return true;
	}

	return false;
}

void APriestAIControllerBase::ChooseNewTarget()
{
	StopUnitMovement();
	AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);
	AUnitBase* ClosestAlly = FindMostWoundedAllyInSight(PerceivedActors);
	ControlledUnit->SetAttackTargetActor(ClosestAlly);

}

void APriestAIControllerBase::CheckIfEnemiesPerceived()
{
	AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);

	if (FindClosestEnemyInSight(PerceivedActors) == nullptr)
	{
		ControlledUnit->SetUnitVisibility(false);
	}
}
