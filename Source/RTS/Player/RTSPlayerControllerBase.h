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
	ARTSPlayerControllerBase();
	
	void AddUnitToSelection(AUnitBase* UnitToAdd);

	void RemoveUnitFromSelection(AUnitBase* UnitToRemove);

	void ClearSelection();

	UPROPERTY(BlueprintReadOnly)
	TArray<AUnitBase*> UnitSelection;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void ZoomCameraIn();

	void ZoomCameraOut();

	ARTSCameraBase* CameraActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float ZoomFactor = 1.0f;

	ARTSCanvasBase* CanvasHUDInstance = nullptr;

	void StartSelection();
	void StopSelection();

	void MoveUnit();

	TArray<AUnitBase*> BindedUnitGroup;

	TArray<TArray<AUnitBase*>> BindedGroups;

	void BindGroup();

	void SelectBindedGroup();

};
