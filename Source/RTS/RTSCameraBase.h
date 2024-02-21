// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSCameraBase.generated.h"

class USceneComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RTS_API ARTSCameraBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARTSCameraBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* RootSceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* Camera = nullptr;

	APlayerController* PlayerController = nullptr;

	void MoveLeft();

	void MoveRight();

	void MoveUp();

	void MoveDown();

	float GetScaledCursorLocation(float CursorLocationToScale);

	float GetViewportSensetiveZone(float ViewportAxis, float ZonePercantage);

	float GetMovementSpeedScaledOnZoom();

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USpringArmComponent* SpringArm = nullptr;

};
