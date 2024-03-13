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

	for (int i = 0; i < 3; i++)
	{
		BindedGroups.Add(BindedUnitGroup);
	}

}

void ARTSPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraOut);
	InputComponent->BindAction("SelectWithCanvas", IE_Pressed, this, &ARTSPlayerControllerBase::StartSelection);
	InputComponent->BindAction("SelectWithCanvas", IE_Released, this, &ARTSPlayerControllerBase::StopSelection);
	InputComponent->BindAction("ExecuteAction", IE_Pressed, this, &ARTSPlayerControllerBase::MoveUnit);
	InputComponent->BindAction("BindUnitGroup", IE_Pressed, this, &ARTSPlayerControllerBase::BindGroup);
	InputComponent->BindAction("SelectBindedGroup", IE_Pressed, this, &ARTSPlayerControllerBase::SelectBindedGroup);
	InputComponent->BindAction("AddMovementWaypoint", IE_Pressed, this, &ARTSPlayerControllerBase::AddMovementWaypoint);
	InputComponent->BindAction("StopMovement", IE_Pressed, this, &ARTSPlayerControllerBase::StopMovement);

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
				UnitController->MovementWaypoints.Empty();
				UnitController->MovementWaypoints.Add(CursorHitResult.Location);
				UnitController->RepositionUnit();
			}

		}

	}
}

void ARTSPlayerControllerBase::BindGroup()
{
	BindedUnitGroup.Empty();
	int32 IndexToBind = -1;
	
	for (AUnitBase* Unit : UnitSelection)
	{
		BindedUnitGroup.Add(Unit);
	}

	if (WasInputKeyJustPressed(FName("One")))
	{
		IndexToBind = 0;
	}
	else if (WasInputKeyJustPressed(FName("Two")))
	{
		IndexToBind = 1;
	}
	else if (WasInputKeyJustPressed(FName("Three")))
	{
		IndexToBind = 2;
	}

	BindedGroups[IndexToBind] = BindedUnitGroup;

}

void ARTSPlayerControllerBase::SelectBindedGroup()
{
	int32 IndexToSelect = -1;
	
	if (WasInputKeyJustPressed(FName("One")) && BindedGroups.IsValidIndex(0))
	{
		IndexToSelect = 0;
	}
	else if (WasInputKeyJustPressed(FName("Two")) && BindedGroups.IsValidIndex(1))
	{
		IndexToSelect = 1;
	}
	else if (WasInputKeyJustPressed(FName("Three")) && BindedGroups.IsValidIndex(2))
	{
		IndexToSelect = 2;
	}

	if (BindedGroups.IsValidIndex(IndexToSelect))
	{
		ClearSelection();

		for (AUnitBase* Unit : BindedGroups[IndexToSelect])
		{
			if (IsValid(Unit))
			{
				AddUnitToSelection(Unit);
			}
		}
	}

}

void ARTSPlayerControllerBase::AddMovementWaypoint()
{
	FHitResult CursorInteractableHitResult;

	if (!GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
	{
		FHitResult CursorHitResult;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, CursorHitResult);

		for (AUnitBase* Unit : UnitSelection)
		{
			ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());

			if (IsValid(UnitController))
			{
				if (UnitController->MovementWaypoints.Num() == 0)
				{
					UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
					UnitBlackboard->SetValueAsVector(FName("TargetLocation"), CursorHitResult.Location);
					UnitController->RepositionUnit();
				}

				UnitController->MovementWaypoints.Add(CursorHitResult.Location);

			}

		}

	}

}

void ARTSPlayerControllerBase::StopMovement()
{
	for (AUnitBase* Unit : UnitSelection)
	{
		ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());
		if (IsValid(UnitController))
		{
			UnitController->StopUnitMovement();
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

