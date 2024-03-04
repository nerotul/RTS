// Fill out your copyright notice in the Description page of Project Settings.


#include "PriestAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "RTS/Units/UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS/GAS/RTSAttributeSet.h"
#include "Components/WidgetComponent.h"

void APriestAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void APriestAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (Stimulus.WasSuccessfullySensed() == true && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit && 
		ControlledUnit->TargetActor == nullptr && SensedUnit->AttributeSet->GetHealth() < SensedUnit->AttributeSet->GetMaxHealth())
	{
		ControlledUnit->SetAttackTargetActor(SensedUnit);

	}
	else if (Stimulus.WasSuccessfullySensed() == false && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit == ControlledUnit->bIsPlayersUnit)
	{
		ChooseNewTarget();
	}

	if (Stimulus.WasSuccessfullySensed() == true && IsValid(SensedUnit) && SensedUnit->bIsAlive == true && SensedUnit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit)
	{
		if (ControlledUnit->bIsPlayersUnit == false && ControlledUnit->bIsAlive == true)
		{
			ControlledUnit->HealthWidget->SetVisibility(true);
			UnitBlackboard->SetValueAsVector(FName("TargetLocation"), ControlledUnit->GetActorLocation());
		}

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
			Unit->AttributeSet->GetHealth() < MinHealth && Unit->AttributeSet->GetHealth() < Unit->AttributeSet->GetMaxHealth())
		{
			MinHealth = Unit->AttributeSet->GetHealth();
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
