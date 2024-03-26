// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerControllerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoldAmountChanged);

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

	UFUNCTION(BlueprintCallable)
	void ChangePlayersGoldAmount(int DeltaGold);

	UFUNCTION(BlueprintCallable)
	int GetPlayersGoldAmount() const;

	UPROPERTY(BlueprintAssignable)
	FOnGoldAmountChanged OnGoldAmountChanged;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void ZoomCameraIn();

	void ZoomCameraOut();

	ARTSCameraBase* CameraActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float ZoomFactor = 1.0f;

	ARTSCanvasBase* CanvasHUDInstance = nullptr;

	void StartCanvasSelection();
	void StopCanvasSelection();

	void ExecuteAction();

	void MoveUnit();

	TArray<AUnitBase*> BindedUnitGroup;

	TArray<TArray<AUnitBase*>> BindedGroups;

	void BindGroup();

	void SelectBindedGroup();

	void AddMovementWaypoint();

	void StopMovement();

	int PlayersGold = 0;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundBase*> CommandSounds;

	FTimerHandle CommandSoundCooloff;

	void PlayCommandSound(AUnitBase* UnitToPlaySoundAt);

	bool bIsReadyToPlayCommandSound = true;

	void FinishCommandSoundCooloff();

	void SelectWithClick();

	void SelectMultipleWithClick();

	void SelectSameClassVisibleUnits();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSelectSameClassUnits(AActor* ClickedUnit);


};
