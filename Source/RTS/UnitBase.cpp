// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "Components/DecalComponent.h"
#include "RTSPlayerControllerBase.h"


// Sets default values
AUnitBase::AUnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnClicked.AddUniqueDynamic(this, &AUnitBase::OnSelected);

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

void AUnitBase::OnSelected(AActor* Target, FKey ButtonPressed)
{
	if (bIsSelected == false)
	{
		bIsSelected = true;
		DecalComponent->SetVisibility(true);
		if (IsValid(RTSPlayerController))
		{
			RTSPlayerController->AddUnitToSelection(this);
		}
	}
	else if (bIsSelected == true)
	{
		bIsSelected = false;
		DecalComponent->SetVisibility(false);

		if (IsValid(RTSPlayerController))
		{
			RTSPlayerController->RemoveUnitFromSelection(this);
		}
	}
}
