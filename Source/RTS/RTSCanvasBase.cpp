// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCanvasBase.h"

void ARTSCanvasBase::StartSelection()
{
	float ScaledCursorLocationX = 0.0f;
	float ScaledCursorLocationY = 0.0f;

	GetScaledCursorPosition(ScaledCursorLocationX, ScaledCursorLocationY);

	PointA.X = ScaledCursorLocationX;
	PointA.Y = ScaledCursorLocationY;
	bIsDrawing = true;

}

void ARTSCanvasBase::StopSelection()
{
	bIsDrawing = false;
}

void ARTSCanvasBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();

}

void ARTSCanvasBase::DrawHUD()
{
	if (bIsDrawing == true)
	{
		float ScaledCursorLocationX = 0.0f;
		float ScaledCursorLocationY = 0.0f;

		GetScaledCursorPosition(ScaledCursorLocationX, ScaledCursorLocationY);

		PointB.X = ScaledCursorLocationX;
		PointB.Y = ScaledCursorLocationY;

		DrawRect(CanvasColor, PointA.X, PointA.Y, (PointB.X - PointA.X), (PointB.Y - PointA.Y));
	}
}

void ARTSCanvasBase::GetScaledCursorPosition(float& OutScaledCursorLocationX, float& OutScaledCursorLocationY)
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	ViewportScale = GEngine->GameViewport->GetDPIScale();

	OutScaledCursorLocationX = CursorLocationX * ViewportScale;
	OutScaledCursorLocationY = CursorLocationY * ViewportScale;

}
