// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSSelectionInterface.h"
#include "RTSCanvasBase.generated.h"


/**
 * 
 */
UCLASS()
class RTS_API ARTSCanvasBase : public AHUD, public IRTSSelectionInterface
{
	GENERATED_BODY()
	
public:
	virtual void StartSelection() override;

	virtual void StopSelection() override;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor CanvasColor;

protected:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;
	
	FVector2D PointA;
	FVector2D PointB;

	APlayerController* PlayerController = nullptr;

	bool bIsDrawing = false;

	void GetScaledCursorPosition(float& OutScaledCursorLocationX, float& OutScaledCursorLocationY);
};
