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

		if (NewValue <= 0)
		{
			if (IsValid(OwnerUnit) && OwnerUnit->bIsAlive == true)
			{
				OwnerUnit->UnitDeath();
			}
		}

		if (NewValue < GetHealth())
		{
			UAISense_Damage::ReportDamageEvent(GetWorld(), OwnerUnit, nullptr, NewValue - GetHealth(), OwnerUnit->GetActorLocation(), OwnerUnit->GetActorLocation());
		}

	}

}

void URTSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	AUnitBase* OwnerUnit = Cast<AUnitBase>(GetOwningActor());
	OwnerUnit->OnHealthChanged();
}
