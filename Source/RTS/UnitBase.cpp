// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "Components/DecalComponent.h"
#include "RTSPlayerControllerBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "RTSAIControllerBase.h"

// Sets default values
AUnitBase::AUnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	OnClicked.AddUniqueDynamic(this, &AUnitBase::OnUnitClicked);

	RTSPlayerController = Cast<ARTSPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
	ThisUnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);

}

// Called every frame
void AUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(ThisUnitBlackboard) && IsValid(GetController()) && TargetActor != nullptr && ThisUnitBlackboard->GetValueAsEnum(FName("ActionEnum")) != 3)
	{
		UpdateTargetLocation();
	}

}

// Called to bind functionality to input
void AUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnitBase::Attack()
{
	TSubclassOf <UDamageType> DamageType;
	UGameplayStatics::ApplyDamage(TargetActor, UnitAttackDamage, GetController(), this, DamageType);
}

void AUnitBase::IsSelected(bool bIsSelected)
{
	DecalComponent->SetVisibility(bIsSelected);
}

void AUnitBase::OnUnitClicked(AActor* Target, FKey ButtonPressed)
{
	if (ButtonPressed.GetFName() == FName("LeftMouseButton") && bIsPlayersUnit == true)
	{
		if (DecalComponent->IsVisible() == false)
		{
			RTSPlayerController->AddUnitToSelection(this);
		}
		else if (DecalComponent->IsVisible() == true)
		{
			RTSPlayerController->RemoveUnitFromSelection(this);
		}

	}
	else if (ButtonPressed.GetFName() == FName("RightMouseButton") && bIsPlayersUnit == false)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

		for (AUnitBase* Unit : RTSPlayerController->UnitSelection)
		{
			FNavLocation RandomNavPoint;

			NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 200.0f, RandomNavPoint);

			Unit->TargetActor = this;
			UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
			UnitBlackboard->SetValueAsVector(FName("TargetLocation"), RandomNavPoint.Location);
			UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 2);
		}

	}
}

// Called in BP construction script to visualize friend/foe status in editor
void AUnitBase::SetFriendFoeDecal()
{
	if (bIsPlayersUnit == true)
	{
		DecalComponent->SetMaterial(0, FriendDecalMaterial);
		DecalComponent->SetVisibility(false);
	}
	else
	{
		DecalComponent->SetMaterial(0, FoeDecalMaterial);
		DecalComponent->SetVisibility(true);
	}
}

float AUnitBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (UnitHealth > 0)
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		UnitHealth -= DamageAmount;
		if (UnitHealth <= 0)
		{
			UnitDeath();
		}

		// Reporting damage event to the AI Perception to start retaliation attack
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator, DamageAmount, DamageCauser->GetActorLocation(), this->GetActorLocation());

		UE_LOG(LogTemp, Warning, TEXT("I'M HIT!"));

	}
	return DamageAmount;
}

void AUnitBase::UnitDeath()
{
	if (IsValid(GetController()))
	{
		GetController()->UnPossess();
		GetWorldTimerManager().SetTimer(DestroyDeadActorTimer, this, &AUnitBase::DestroyDeadActor, 1.0f, false, 5.0f);

		// If unit is friendly and was selected
		if (bIsPlayersUnit == true && DecalComponent->IsVisible())
		{
			RTSPlayerController->RemoveUnitFromSelection(this);
		}
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->PlayAnimation(DeathAnimation, false);

}

void AUnitBase::DestroyDeadActor()
{
	Destroy();
}

void AUnitBase::UpdateTargetLocation()
{
	if (GetDistanceTo(TargetActor) <= AttackRadius)
	{
		ThisUnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 1);
	}
	else if(GetDistanceTo(TargetActor) < StopChaseRadius)
	{
		ThisUnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 2);
		ThisUnitBlackboard->SetValueAsVector(FName("TargetLocation"), TargetActor->GetActorLocation());
	}
	else if (GetDistanceTo(TargetActor) >= StopChaseRadius)
	{
		ARTSAIControllerBase* AIController = Cast<ARTSAIControllerBase>(GetController());
		AIController->ChooseNewTarget();
	}

	AUnitBase* TargetUnit = Cast<AUnitBase>(TargetActor);

	if (IsValid(TargetUnit) && TargetUnit->UnitHealth < 0)
	{
		ARTSAIControllerBase* AIController = Cast<ARTSAIControllerBase>(GetController());
		ThisUnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 0);
		AIController->ChooseNewTarget();
	}

}
