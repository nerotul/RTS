// Fill out your copyright notice in the Description page of Project Settings.


#include "PriestBase.h"
#include "Components/DecalComponent.h"
#include "RTS/Player/RTSPlayerControllerBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "RTS/AI/RTSAIControllerBase.h"
#include "Components/CapsuleComponent.h"

void APriestBase::BeginPlay()
{
	Super::BeginPlay();
}