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
#include "Kismet/GameplayStatics.h"
#include "RTS/Units/PriestBase.h"

ARTSPlayerControllerBase::ARTSPlayerControllerBase()
{
	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	// Needed for unit to react on LMB click
	DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;  
}

void ARTSPlayerControllerBase::ChangePlayersGoldAmount(int DeltaGold)
{
	PlayersGold += DeltaGold;
	OnGoldAmountChanged.Broadcast();
}

int ARTSPlayerControllerBase::GetPlayersGoldAmount() const
{
	return PlayersGold;
}

void ARTSPlayerControllerBase::SelectUnitWithWidgetClick(AUnitBase* SelectedUnit)
{
	if (IsValid(SelectedUnit) && SelectedUnit->bIsAlive == true)
	{
		ClearSelection();
		AddUnitToSelection(SelectedUnit);
		OnUnitSelectionChanged.Broadcast();
	}
}

int32 ARTSPlayerControllerBase::GetBindedGroupValidUnitAmount(int32 GroupIndex)
{
	for (AUnitBase* Unit : BindedGroups[GroupIndex])
	{
		if (IsValid(Unit) && Unit->bIsAlive == false)
		{
			BindedGroups[GroupIndex].Remove(Unit);
		}
	}

	return BindedGroups[GroupIndex].Num();
}

void ARTSPlayerControllerBase::SelectBindedGroupWithWidgetClick(int32 GroupIndex)
{
	if (BindedGroups.IsValidIndex(GroupIndex))
	{
		ClearSelection();

		for (AUnitBase* Unit : BindedGroups[GroupIndex])
		{
			if (IsValid(Unit))
			{
				AddUnitToSelection(Unit);
			}
		}
	}

	OnUnitSelectionChanged.Broadcast();

}

TArray<AUnitBase*> ARTSPlayerControllerBase::GetBindedUnitGroupByIndex(int32 Index)
{
	return BindedGroups[Index];
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
	InputComponent->BindAction("SelectWithCanvas", IE_Pressed, this, &ARTSPlayerControllerBase::StartCanvasSelection);
	InputComponent->BindAction("SelectWithCanvas", IE_Released, this, &ARTSPlayerControllerBase::StopCanvasSelection);
	InputComponent->BindAction("ExecuteAction", IE_Pressed, this, &ARTSPlayerControllerBase::ExecuteAction);
	InputComponent->BindAction("BindUnitGroup", IE_Pressed, this, &ARTSPlayerControllerBase::BindGroup);
	InputComponent->BindAction("SelectBindedGroup", IE_Pressed, this, &ARTSPlayerControllerBase::SelectBindedGroup);
	InputComponent->BindAction("AddMovementWaypoint", IE_Pressed, this, &ARTSPlayerControllerBase::AddMovementWaypoint);
	InputComponent->BindAction("StopMovement", IE_Pressed, this, &ARTSPlayerControllerBase::StopUnitMovement);
	InputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &ARTSPlayerControllerBase::MouseLeftClick);
	InputComponent->BindAction("SelectMultipleWithClick", IE_Pressed, this, &ARTSPlayerControllerBase::SelectMultipleWithClick);
	InputComponent->BindAction("SelectSameClassVisibleUnits", IE_Pressed, this, &ARTSPlayerControllerBase::SelectSameClassVisibleUnits);
	InputComponent->BindAction("SetCursorModeToMove", IE_Pressed, this, &ARTSPlayerControllerBase::SetCursorModeToMove);
	InputComponent->BindAction("SetCursorModeToAttack", IE_Pressed, this, &ARTSPlayerControllerBase::SetCursorModeToAttack);
	
}

void ARTSPlayerControllerBase::ZoomCameraIn()
{
	if (IsValid(CameraActor))
	{
		float SpringArmLenghOldValue = CameraActor->SpringArm->TargetArmLength;
		float SpringArmLenghNewValue = SpringArmLenghOldValue - ZoomFactor * 200.0f;
		float ClampedNewValue = FMath::Clamp(SpringArmLenghNewValue, 1000.0f, 3000.0f);
		CameraActor->SpringArm->TargetArmLength = ClampedNewValue;
	}
}

void ARTSPlayerControllerBase::ZoomCameraOut()
{
	if (IsValid(CameraActor))
	{
		float SpringArmLenghOldValue = CameraActor->SpringArm->TargetArmLength;
		float SpringArmLenghNewValue = ZoomFactor * 200.0f + SpringArmLenghOldValue;
		float ClampedNewValue = FMath::Clamp(SpringArmLenghNewValue, 1000.0f, 3000.0f);
		CameraActor->SpringArm->TargetArmLength = ClampedNewValue;

	}

}

void ARTSPlayerControllerBase::StartCanvasSelection()
{
	if (IsValid(CanvasHUDInstance))
	{
		CanvasHUDInstance->StartSelection();
	}
}

void ARTSPlayerControllerBase::StopCanvasSelection()
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

		if (UnitSelection.Num() > 0)
		{
			PlayCommandSound(UnitSelection[0]);

			for (AUnitBase* Unit : UnitSelection)
			{
				if (IsValid(Unit) && Unit->bIsAlive == true)
				{
					UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
					if (IsValid(UnitBlackboard))
					{
						UnitBlackboard->SetValueAsVector(FName("TargetLocation"), CursorHitResult.Location);
					}

					ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());
					if (IsValid(UnitController))
					{
						UnitController->MovementWaypoints.Empty();
						UnitController->MovementWaypoints.Add(CursorHitResult.Location);

						if (CurrentCursorMode == ECursorMode::Default || CurrentCursorMode == ECursorMode::Move)
						{
							UnitController->RepositionUnit();
						}
						else if (CurrentCursorMode == ECursorMode::Attack)
						{
							UnitController->FinishRepositionUnit();
						}
					}

				}

			}

			SetCursorModeToDefault();

		}

	}
}

void ARTSPlayerControllerBase::BindGroup()
{
	BindedUnitGroup.Empty();
	int32 IndexToBind = -1;
	
	for (AUnitBase* Unit : UnitSelection)
	{
		if (IsValid(Unit))
		{
			BindedUnitGroup.Add(Unit);
		}
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

	OnBindedGroupsChanged.Broadcast(IndexToBind);

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

	OnUnitSelectionChanged.Broadcast();

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
			if(IsValid(Unit))
			{
				ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());

				if (IsValid(UnitController))
				{
					if (UnitController->MovementWaypoints.Num() == 0)
					{
						UBlackboardComponent* UnitBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(Unit);
						if (IsValid(UnitBlackboard))
						{
							UnitBlackboard->SetValueAsVector(FName("TargetLocation"), CursorHitResult.Location);
						}

						UnitController->RepositionUnit();
					}

					UnitController->MovementWaypoints.Add(CursorHitResult.Location);

				}
			}

		}

	}

}

void ARTSPlayerControllerBase::ExecuteAction()
{
	if (CurrentCursorMode == ECursorMode::Default)
	{
		FHitResult CursorInteractableHitResult;

		if (!GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
		{
			MoveUnit();
		}
		else if (GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
		{
			AActor* CursorHitActor = CursorInteractableHitResult.GetActor();
			AUnitBase* CursorHitUnit = Cast<AUnitBase>(CursorHitActor);

			if (IsValid(CursorHitUnit) && CursorHitUnit->bIsPlayersUnit == false && CursorHitUnit->bIsAlive == true)
			{
				for (AUnitBase* Unit : UnitSelection)
				{
					if (IsValid(Unit))
					{
						APriestBase* Priest = Cast<APriestBase>(Unit);
						if (!IsValid(Priest))
						{
							Unit->SetAttackTargetActor(CursorHitUnit);
						}

					}
				}
			}
			else if (IsValid(CursorHitUnit) && CursorHitUnit->bIsPlayersUnit == true && CursorHitUnit->bIsAlive == true)
			{
				for (AUnitBase* Unit : UnitSelection)
				{
					if (IsValid(Unit))
					{
						APriestBase* Priest = Cast<APriestBase>(Unit);
						if (IsValid(Priest))
						{
							Unit->SetAttackTargetActor(CursorHitUnit);

						}

					}
				}
			}
		}

	}
	else
	{
		SetCursorModeToDefault();
	}
}

void ARTSPlayerControllerBase::StopUnitMovement()
{
	for (AUnitBase* Unit : UnitSelection)
	{
		if (IsValid(Unit))
		{
			ARTSAIControllerBase* UnitController = Cast<ARTSAIControllerBase>(Unit->GetController());
			if (IsValid(UnitController))
			{
				UnitController->StopUnitMovement();
			}

		}
	}

}

void ARTSPlayerControllerBase::SetCursorModeToMove()
{
	CurrentCursorMode = ECursorMode::Move;
	this->CurrentMouseCursor = EMouseCursor::Crosshairs;

}

void ARTSPlayerControllerBase::SetCursorModeToDefault()
{
	CurrentCursorMode = ECursorMode::Default;
	this->CurrentMouseCursor = EMouseCursor::Default;

}

void ARTSPlayerControllerBase::SetCursorModeToAttack()
{
	CurrentCursorMode = ECursorMode::Attack;
	this->CurrentMouseCursor = EMouseCursor::CardinalCross;

}

void ARTSPlayerControllerBase::MouseLeftClick()
{
	switch (CurrentCursorMode)
	{
	case ECursorMode::Default:
		SelectWithClick();
		break;
	case ECursorMode::Move:
		MoveUnit();
		break;
	case ECursorMode::Attack:
		MoveUnit();
		break;
	default:
		break;
	}

}

void ARTSPlayerControllerBase::PlayCommandSound(AUnitBase* UnitToPlaySoundAt)
{
	if (bIsReadyToPlayCommandSound == true && IsValid(UnitToPlaySoundAt))
	{
		bIsReadyToPlayCommandSound = false;
		GetWorldTimerManager().SetTimer(CommandSoundCooloff, this, &ARTSPlayerControllerBase::FinishCommandSoundCooloff, 1.0f, false, 4.0f);

		int32 SoundToPlayIndex = FMath::RandRange(0, CommandSounds.Num() - 1);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CommandSounds[SoundToPlayIndex], UnitToPlaySoundAt->GetActorLocation());

	}

}

void ARTSPlayerControllerBase::FinishCommandSoundCooloff()
{
	bIsReadyToPlayCommandSound = true;
}

void ARTSPlayerControllerBase::ClearSelection()
{
	for (AUnitBase* Unit : UnitSelection)
	{
		if (IsValid(Unit))
		{
			Unit->IsSelected(false);

		}
	}

	UnitSelection.Empty();
	OnUnitSelectionChanged.Broadcast();
}

void ARTSPlayerControllerBase::AddUnitToSelection(AUnitBase* UnitToAdd)
{
	if (IsValid(UnitToAdd) && UnitToAdd->AttributeSet->GetHealth() > 0)
	{
		UnitSelection.AddUnique(UnitToAdd);
		UnitToAdd->IsSelected(true);
		OnUnitSelectionChanged.Broadcast();

	}

}

void ARTSPlayerControllerBase::RemoveUnitFromSelection(AUnitBase* UnitToRemove)
{
	if (IsValid(UnitToRemove))
	{
		UnitSelection.Remove(UnitToRemove);
		UnitToRemove->IsSelected(false);
		OnUnitSelectionChanged.Broadcast();

	}

}

void ARTSPlayerControllerBase::SelectWithClick()
{
	FHitResult CursorInteractableHitResult;

	if (GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
	{
		AActor* CursorHitActor = CursorInteractableHitResult.GetActor();
		AUnitBase* CursorHitUnit = Cast<AUnitBase>(CursorHitActor);

		if (IsValid(CursorHitUnit) && CursorHitUnit->bIsAlive == true)
		{
			ClearSelection();
			AddUnitToSelection(CursorHitUnit);
			OnUnitSelectionChanged.Broadcast();

		}
	}

}

void ARTSPlayerControllerBase::SelectSameClassVisibleUnits()
{
	FHitResult CursorInteractableHitResult;

	if (GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
	{
		ClearSelection();
		TArray<AActor*> FoundActors;
		AActor* CursorHitActor = CursorInteractableHitResult.GetActor();
		if (IsValid(CursorHitActor))
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), CursorHitActor->GetClass(), FoundActors);
		}

		for (AActor* Actor : FoundActors)
		{
			AUnitBase* Unit = Cast<AUnitBase>(Actor);

			if (IsValid(Unit))
			{
				if (Unit->bIsAlive == true && Unit->bIsPlayersUnit == true)
				{
					AddUnitToSelection(Unit);
				}
			}

		}

		OnUnitSelectionChanged.Broadcast();


	}
}

void ARTSPlayerControllerBase::SelectMultipleWithClick()
{
	FHitResult CursorInteractableHitResult;

	if (GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, CursorInteractableHitResult))
	{
		AActor* CursorHitActor = CursorInteractableHitResult.GetActor();
		AUnitBase* CursorHitUnit = Cast<AUnitBase>(CursorHitActor);

		if (IsValid(CursorHitUnit) && CursorHitUnit->bIsAlive == true)
		{
			if (CursorHitUnit->DecalComponent->IsVisible() == false)
			{
				AddUnitToSelection(CursorHitUnit);
				OnUnitSelectionChanged.Broadcast();

			}
			else if (CursorHitUnit->DecalComponent->IsVisible() == true)
			{
				RemoveUnitFromSelection(CursorHitUnit);
				OnUnitSelectionChanged.Broadcast();

			}
		}
	}
}
