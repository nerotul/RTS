// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoreBase.generated.h"

class UWidgetComponent;
class AUnitBase;

UCLASS()
class RTS_API AStoreBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoreBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootSceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AUnitBase>> UnitsForSale;

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnitBought(const AUnitBase* UnitBought);

	UPROPERTY(EditDefaultsOnly)
	USoundBase* PurchaseSound = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int PurchaseCost = 20;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TryBuyUnit(int32 UnitsForSaleIndex);

};
