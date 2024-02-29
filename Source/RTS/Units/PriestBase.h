// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "PriestBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API APriestBase : public AUnitBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void OnUnitClicked(AActor* Target, FKey ButtonPressed) override;

};
