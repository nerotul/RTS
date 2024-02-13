// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerControllerBase.h"
#include "RTSCameraBase.h"
#include "GameFramework/SpringArmComponent.h"



ARTSPlayerControllerBase::ARTSPlayerControllerBase()
{
	SetShowMouseCursor(true);
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
}

void ARTSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);

	AActor* ViewTarget = GetViewTarget();
	CameraActor = Cast<ARTSCameraBase>(ViewTarget);

}

void ARTSPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSPlayerControllerBase::ZoomCameraOut);

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

