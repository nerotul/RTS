// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "Components/DecalComponent.h"
#include "RTSPlayerControllerBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

// Sets default values
AUnitBase::AUnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

}

void AUnitBase::IsSelected(bool bIsSelected)
{
	DecalComponent->SetVisibility(bIsSelected);
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

void AUnitBase::Attack()
{
	TSubclassOf <UDamageType> DamageType;
	UGameplayStatics::ApplyDamage(TargetActor, 10.0f, GetController(), this, DamageType);
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
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->PlayAnimation(DeathAnimation, false);

}

// Called when the game starts or when spawned
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnClicked.AddUniqueDynamic(this, &AUnitBase::OnUnitClicked);

	RTSPlayerController = Cast<ARTSPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void AUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

			NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 50.0f, RandomNavPoint);

			Unit->TargetActor = this;
			UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
			UnitBlackboard->SetValueAsVector(FName("TargetLocation"), RandomNavPoint.Location);

			UnitBlackboard->SetValueAsEnum(FName("ActionEnum"), 1);
		}

	}
}
