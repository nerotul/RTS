// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ARTSPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

	ARTSPlayerControllerBase();
	
	virtual void BeginPlay() override;

};
