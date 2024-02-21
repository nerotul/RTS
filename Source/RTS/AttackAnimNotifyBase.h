// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackAnimNotifyBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API UAttackAnimNotifyBase : public UAnimNotify
{
	GENERATED_BODY()

public:

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
