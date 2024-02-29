// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAttributeSet.h"

URTSAttributeSet::URTSAttributeSet()
{
}

void URTSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PreAttribute!"));

}
