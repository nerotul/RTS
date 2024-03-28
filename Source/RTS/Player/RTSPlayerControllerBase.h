// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerControllerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoldAmountChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitSelectionChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBindedGroupsChanged, int32, BindedGroupIndex);

class ARTSCameraBase;
class ARTSCanvasBase;
class AUnitBase;

UENUM(BlueprintType)
enum class ECursorMode : uint8 {
	Default       UMETA(DisplayName = "Default"),
	Move			UMETA(DisplayName = "Move"),
	Attack        UMETA(DisplayName = "Attack"),
};

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

	UPROPERTY(BlueprintAssignable)
	FOnUnitSelectionChanged OnUnitSelectionChanged;

	UPROPERTY(BlueprintAssignable)
	FOnBindedGroupsChanged OnBindedGroupsChanged;

	UFUNCTION(BlueprintCallable)
	void SelectUnitWithWidgetClick(AUnitBase* SelectedUnit);

	UFUNCTION(BlueprintCallable)
	int32 GetBindedGroupValidUnitAmount(int32 GroupIndex);

	UFUNCTION(BlueprintCallable)
	void SelectBindedGroupWithWidgetClick(int32 GroupIndex);

	UFUNCTION(BlueprintCallable)
	TArray<AUnitBase*> GetBindedUnitGroupByIndex(int32 Index);

	UFUNCTION(BlueprintCallable)
	void StopUnitMovement();

	UFUNCTION(BlueprintCallable)
	void SetCursorModeToDefault();

	UFUNCTION(BlueprintCallable)
	void SetCursorModeToMove();

	UFUNCTION(BlueprintCallable)
	void SetCursorModeToAttack();

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

	void BindGroup();

	TArray<AUnitBase*> BindedUnitGroup;

	TArray<TArray<AUnitBase*>> BindedGroups;

	void SelectBindedGroup();

	void AddMovementWaypoint();

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

	ECursorMode CurrentCursorMode = ECursorMode::Default;

	void MouseLeftClick();

};
