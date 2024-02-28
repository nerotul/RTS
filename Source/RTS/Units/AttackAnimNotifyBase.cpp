// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAnimNotifyBase.h"
#include "UnitBase.h"

void UAttackAnimNotifyBase::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AUnitBase* Unit = Cast<AUnitBase>(MeshComp->GetOwner());

	if (IsValid(Unit))
	{
		Unit->Attack();
	}
}
