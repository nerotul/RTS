// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS/Units/StoreBase.h"
#include "Components/WidgetComponent.h"
#include "RTS/Units/UnitBase.h"
#include "Kismet/GameplayStatics.h"
#include "RTS/Player/RTSPlayerControllerBase.h"

// Sets default values
AStoreBase::AStoreBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMesh->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AStoreBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AStoreBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStoreBase::TryBuyUnit(int32 UnitsForSaleIndex)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ARTSPlayerControllerBase* RTSController = Cast<ARTSPlayerControllerBase>(PlayerController);

	if (IsValid(RTSController) && UnitsForSale.IsValidIndex(UnitsForSaleIndex) && RTSController->GetPlayersGoldAmount() >= PurchaseCost)
	{
		FTransform SpawnTransform;
		FVector SpawnLocation(GetActorLocation().X + 200.0f, GetActorLocation().Y + 200.0f, 108.0f);
		SpawnTransform.SetLocation(SpawnLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AUnitBase* SpawnedUnit = GetWorld()->SpawnActor<AUnitBase>(UnitsForSale[UnitsForSaleIndex], SpawnTransform, SpawnParams);

		OnUnitBought(SpawnedUnit);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PurchaseSound, GetActorLocation());
		RTSController->ChangePlayersGoldAmount(-PurchaseCost);

	}
}

