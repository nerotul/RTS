// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerControllerBase.h"
#include "RTSCameraBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "RTSCanvasBase.h"
#include "RTS/Units/UnitBase.h"
#include "Components/DecalComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RTS/AI/RTSAIControllerBase.h"
#include "RTS/GAS/RTSAttributeSet.h"

ARTSPlayerControllerBase::ARTSPlayerControllerBase()
{
	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	// Needed for unit to react on LMB click
	DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;  
}

void ARTSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	// Locks cursor inside game window
	SetInputMode(InputMode);  

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
	InputComponent->BindAction("ExecuteAction", IE_Pressed, this, &ARTSPlayerControllerBase::MoveUnit);

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

void ARTSPlayerControllerBase::MoveUnit()
{
	FHitResult CursorInteractableHitResult;
	// GameTraceChannel1 is a custom "Interactable" channel
	if (!GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
	{
		FHitResult CursorHitResult;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, CursorHitResult);

		for (AUnitBase* Unit : UnitSelection)
		{
			UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
			UnitBlackboard->SetValueAsVector(FName("TargetLocation"), CursorHitResult.Location);

			ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());
			if (IsValid(UnitController))
			{
				UnitController->RepositionUnit();
			}

		}

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
	if (UnitToAdd->AttributeSet->GetHealth() > 0)
	{
		UnitSelection.AddUnique(UnitToAdd);
		UnitToAdd->IsSelected(true);

	}

}

void ARTSPlayerControllerBase::RemoveUnitFromSelection(AUnitBase* UnitToRemove)
{
	UnitSelection.Remove(UnitToRemove);
	UnitToRemove->IsSelected(false);

}

