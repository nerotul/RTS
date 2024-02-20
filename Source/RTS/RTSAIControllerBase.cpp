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
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ARTSAIControllerBase::EnemySensed);

}

void ARTSAIControllerBase::EnemySensed(AActor* SensedActor, FAIStimulus Stimulus)
{
	AUnitBase* SensedUnit = Cast<AUnitBase>(SensedActor);

	if (ControlledUnit != nullptr && IsValid(SensedUnit) && SensedUnit->bIsPlayersUnit != ControlledUnit->bIsPlayersUnit)
	{
		ControlledUnit->TargetActor = SensedActor;
		UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
		UnitBlackboard->SetValueAsVector(FName("TargetLocation"), SensedActor->GetActorLocation());
		UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 1);
		UE_LOG(LogTemp, Warning, TEXT("Sensed enemy!"));
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
	UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 1);


}
