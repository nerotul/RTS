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

}

void ARTSAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (ControlledUnit != nullptr && IsValid(SensedUnit) && SensedUnit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit)
	{
		ControlledUnit->TargetActor = SensedActor;
		UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
		UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 2);
		UnitBlackboard->SetValueAsObject(FName("AttackTargetActor"), SensedUnit);
	}

}

void ARTSAIControllerBase::SightCooloff()
{
	AIPerceptionComponent->SetSenseEnabled(SightSenseConfig->GetSenseImplementation(), false);
	GetWorldTimerManager().SetTimer(SightCooloffTimer, this, &ARTSAIControllerBase::EnableSightSense, 1.0f, false, 2.0f);
}

void ARTSAIControllerBase::EnableSightSense()
{
	AIPerceptionComponent->SetSenseEnabled(SightSenseConfig->GetSenseImplementation(), true);
	UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 2);


}

void ARTSAIControllerBase::ChooseNewTarget()
{
	AIPerceptionComponent->GetCurrentlyPerceivedActors(SightSenseConfig->GetSenseImplementation(), PerceivedActors);

	float MinDistance = 99999.0f;
	AUnitBase* ClosestEnemy = nullptr;

	for (AActor* Actor : PerceivedActors)
	{
		AUnitBase* Unit = Cast<AUnitBase>(Actor);
		if (IsValid(Unit) && Unit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit && ControlledUnit->GetDistanceTo(Unit) < MinDistance)
		{
			MinDistance = ControlledUnit->GetDistanceTo(Unit);
			ClosestEnemy = Unit;
		}
	}

	if (ClosestEnemy != nullptr)
	{
		ControlledUnit->TargetActor = ClosestEnemy;
		UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
		UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 2);
		UnitBlackboard->SetValueAsObject(FName("AttackTargetActor"), ClosestEnemy);

	}
	else
	{
		UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
		UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 0);
		UnitBlackboard->SetValueAsVector(FName("TargetLocation"), ControlledUnit->GetActorLocation());
		UnitBlackboard->SetValueAsObject(FName("AttackTargetActor"), nullptr);

	}

}

