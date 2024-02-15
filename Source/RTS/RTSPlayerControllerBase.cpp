// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerControllerBase.h"
#include "RTSCameraBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "RTSCanvasBase.h"
#include "UnitBase.h"
#include "Components/DecalComponent.h"

ARTSPlayerControllerBase::ARTSPlayerControllerBase()
{
	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableTouchEvents = true;

	DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;  // Needed for unit to react on LMB click
}

void ARTSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);  // Locks cursor inside game window

	AActor* ViewTarget = GetViewTarget();
	CameraActor = Cast<ARTSCameraBase>(ViewTarget);

	AHUD* HUDInstance = GetHUD();
	CanvasHUDInstance = Cast<ARTSCanvasBase>(HUDInstance);

	

}

void ARTSPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraOut);
	InputComponent->BindAction("SelectWithCanvas", IE_Pressed, this, &ARTSPlayerControllerBase::StartSelection);
	InputComponent->BindAction("SelectWithCanvas", IE_Released, this, &ARTSPlayerControllerBase::StopSelection);

}

void ARTSPlayerControllerBase::ZoomCameraIn()
{
	if (IsValid(CameraActor))
	{
		float SpringArmLenghOldValue = CameraActor->SpringArm->TargetArmLength;
		float SpringArmLenghNewValue = SpringArmLenghOldValue - ZoomFactor * 200.0f;
		float ClampedNewValue = FMath::Clamp(SpringArmLenghNewValue, 1000.0f, 5000.0f);
		CameraActor->SpringArm->TargetArmLength = ClampedNewValue;
	}
}

void ARTSPlayerControllerBase::ZoomCameraOut()
{
	if (IsValid(CameraActor))
	{
		float SpringArmLenghOldValue = CameraActor->SpringArm->TargetArmLength;
		float SpringArmLenghNewValue = ZoomFactor * 200.0f + SpringArmLenghOldValue;
		float ClampedNewValue = FMath::Clamp(SpringArmLenghNewValue, 1000.0f, 5000.0f);
		CameraActor->SpringArm->TargetArmLength = ClampedNewValue;

	}

}

void ARTSPlayerControllerBase::StartSelection()
{
	if (IsValid(CanvasHUDInstance))
	{
		CanvasHUDInstance->StartSelection();
	}
}

void ARTSPlayerControllerBase::StopSelection()
{
	if (IsValid(CanvasHUDInstance))
	{
		CanvasHUDInstance->StopSelection();
	}

}

void ARTSPlayerControllerBase::ClearSelection()
{
	for (AUnitBase* Unit : UnitSelection)
	{
		Unit->IsSelected(false);
	}

	UnitSelection.Empty();
}

void ARTSPlayerControllerBase::AddUnitToSelection(AUnitBase* UnitToAdd)
{
	UnitSelection.AddUnique(UnitToAdd);
	UnitToAdd->IsSelected(true);

}

void ARTSPlayerControllerBase::RemoveUnitFromSelection(AUnitBase* UnitToRemove)
{
	UnitSelection.Remove(UnitToRemove);
	UnitToRemove->IsSelected(false);

}

