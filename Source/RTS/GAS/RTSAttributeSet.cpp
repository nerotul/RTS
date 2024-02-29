// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAttributeSet.h"
#include "RTS/Units/UnitBase.h"

URTSAttributeSet::URTSAttributeSet()
{
}

void URTSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PreAttribute!"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(GetHealth()));


	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());

		if (NewValue == 0)
		{
			AUnitBase* OwnerUnit = Cast<AUnitBase>(GetOwningActor());
			if(IsValid(OwnerUnit))
			{
				OwnerUnit->UnitDeath();
			}
		}
	}

}
