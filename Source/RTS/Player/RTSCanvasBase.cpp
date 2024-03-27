// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCanvasBase.h"
#include "RTS/Units/UnitBase.h"
#include "RTSPlayerControllerBase.h"

void ARTSCanvasBase::BeginPlay()
{
	Super::BeginPlay();

	RTSPlayerController = Cast<ARTSPlayerControllerBase>(GetWorld()->GetFirstPlayerController());

}

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
	FVector2D VectorDif = PointA - PointB;
	float VectorDifLengh = VectorDif.Size();

	bIsDrawing = false;

	if (VectorDifLengh > CanvasDeadZone && IsValid(RTSPlayerController))
	{
		RTSPlayerController->ClearSelection();

		for (AUnitBase* Unit : UnitsSelectedWithRectange)
		{
			if (Unit->bIsPlayersUnit == true && Unit->bIsAlive == true)
			{
				RTSPlayerController->AddUnitToSelection(Unit);

			}
		}

		RTSPlayerController->OnUnitSelectionChanged.Broadcast();

	}

	UnitsSelectedWithRectange.Empty();

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

		FVector2D VectorDif = PointA - PointB;
		float VectorDifLengh = VectorDif.Size();

		if (VectorDifLengh > CanvasDeadZone && IsValid(RTSPlayerController))
		{
			DrawRect(CanvasColor, PointA.X, PointA.Y, (PointB.X - PointA.X), (PointB.Y - PointA.Y));
			GetActorsInSelectionRectangle<AUnitBase>(PointA, PointB, UnitsSelectedWithRectange, true, false);

		}

	}
}

void ARTSCanvasBase::GetScaledCursorPosition(float& OutScaledCursorLocationX, float& OutScaledCursorLocationY)
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;

	if (IsValid(RTSPlayerController))
	{
		RTSPlayerController->GetMousePosition(CursorLocationX, CursorLocationY);

	}
	ViewportScale = GEngine->GameViewport->GetDPIScale();

	OutScaledCursorLocationX = CursorLocationX * ViewportScale;
	OutScaledCursorLocationY = CursorLocationY * ViewportScale;

}
