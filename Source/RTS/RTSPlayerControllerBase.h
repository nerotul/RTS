// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerControllerBase.generated.h"


class ARTSCameraBase;
class ARTSCanvasBase;
class AUnitBase;

/**
 * 
 */
UCLASS()
class RTS_API ARTSPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	void AddUnitToSelection(AUnitBase* UnitToAdd);

	void RemoveUnitFromSelection(AUnitBase* UnitToRemove);


protected:
	virtual void BeginPlay() override;

private:
	ARTSPlayerControllerBase();

	virtual void SetupInputComponent() override;

	void ZoomCameraIn();

	void ZoomCameraOut();

	ARTSCameraBase* CameraActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float ZoomFactor = 1.0f;

	ARTSCanvasBase* CanvasHUDInstance = nullptr;

	void StartSelection();
	void StopSelection();

	TArray<AUnitBase*> UnitSelection;

};
