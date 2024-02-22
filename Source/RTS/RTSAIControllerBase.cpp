// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "UnitBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

ARTSAIControllerBase::ARTSAIControllerBase()
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

	AIPerceptionComponent->SetDominantSense(DamageSenseConfig->GetSenseImplementation());
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
	if (Stimulus.WasSuccessfullySensed() == true)
	{
		AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

		if (ControlledUnit != nullptr && IsValid(SensedUnit) && IsValid(SensedUnit->GetController()) && SensedUnit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit)
		{
			ControlledUnit->SetAttackTargetActor(SensedUnit);
			UE_LOG(LogTemp, Warning, TEXT("Sensed!"));

		}

	}
	else
	{
		ControlledUnit->SetAttackTargetActor(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Lost!"));

	}


}

void ARTSAIControllerBase::EnableSightSense()
{
	AIPerceptionComponent->SetSenseEnabled(SightSenseConfig->GetSenseImplementation(), true);
	UnitBlackboard->SetValueAsBool(FName("bIsRepositioning"), false);
	ChooseNewTarget();
}

void ARTSAIControllerBase::RepositionUnit()
{
	AIPerceptionComponent->SetSenseEnabled(SightSenseConfig->GetSenseImplementation(), false);
	UnitBlackboard->SetValueAsBool(FName("bIsRepositioning"), true);
	GetWorldTimerManager().SetTimer(SightCooloffTimer, this, &ARTSAIControllerBase::EnableSightSense, 1.0f, false, 1.5f);
}

void ARTSAIControllerBase::ChooseNewTarget()
{
	AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);

	float MinDistance = 99999.0f;
	AUnitBase* ClosestEnemy = nullptr;

	for (AActor* Actor : PerceivedActors)
	{
		AUnitBase* Unit = Cast<AUnitBase>(Actor);
		if (IsValid(Unit) && IsValid(Unit->GetController()) && Unit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit && ControlledUnit->GetDistanceTo(Unit) < MinDistance)
		{
			MinDistance = ControlledUnit->GetDistanceTo(Unit);
			ClosestEnemy = Unit;
		}
	}

	ControlledUnit->SetAttackTargetActor(ClosestEnemy);

	if (ClosestEnemy == nullptr)
	{
		UnitBlackboard->SetValueAsVector(FName("TargetLocation"), ControlledUnit->GetActorLocation());
	}

	PerceivedActors.Empty();
}

