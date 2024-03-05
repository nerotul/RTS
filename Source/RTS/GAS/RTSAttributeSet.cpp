// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAttributeSet.h"
#include "RTS/Units/UnitBase.h"
#include "Perception/AISense_Damage.h"

URTSAttributeSet::URTSAttributeSet()
{
}

void URTSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		AUnitBase* OwnerUnit = Cast<AUnitBase>(GetOwningActor());
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());

		// Move these checks to que
		if (NewValue < GetHealth())
		{
			UAISense_Damage::ReportDamageEvent(GetWorld(), OwnerUnit, nullptr, NewValue - GetHealth(), OwnerUnit->GetActorLocation(), OwnerUnit->GetActorLocation());
		}

		if (NewValue == 0)
		{

			if(IsValid(OwnerUnit) && OwnerUnit->bIsAlive == true)
			{
				OwnerUnit->UnitDeath();
			}
		}
	}

}
