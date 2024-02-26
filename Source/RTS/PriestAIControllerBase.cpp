// Fill out your copyright notice in the Description page of Project Settings.


#include "PriestAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"


void APriestAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void APriestAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (Stimulus.WasSuccessfullySensed() == true && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit && 
		ControlledUnit->TargetActor == nullptr && SensedUnit->UnitCurrentHealth < SensedUnit->UnitMaxHealth)
	{
		ControlledUnit->SetAttackTargetActor(SensedUnit);

	}
	else if (Stimulus.WasSuccessfullySensed() == false && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit)
	{
		ChooseNewTarget();
	}

}

void APriestAIControllerBase::ChooseNewTarget()
{
	AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);

	float MinHealth = 99999.0f;
	AUnitBase* ClosestAlly = nullptr;

	for (AActor* Actor : PerceivedActors)
	{
		AUnitBase* Unit = Cast<AUnitBase>(Actor);
		if (IsValid(Unit) && Unit->bIsAlive == true && Unit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit && 
			Unit->UnitCurrentHealth < MinHealth && Unit->UnitCurrentHealth < Unit->UnitMaxHealth)
		{
			MinHealth = Unit->UnitCurrentHealth;
			ClosestAlly = Unit;
		}
	}

	ControlledUnit->SetAttackTargetActor(ClosestAlly);

	if (ClosestAlly == nullptr)
	{
		UnitBlackboard->SetValueAsVector(FName("TargetLocation"), ControlledUnit->GetActorLocation());
	}

	PerceivedActors.Empty();

}
