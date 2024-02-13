// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerControllerBase.h"

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


}
