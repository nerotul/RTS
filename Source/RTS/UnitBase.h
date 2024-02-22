// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnitBase.generated.h"

class ARTSPlayerControllerBase;
class UBlackboardComponent;
class ARTSAIControllerBase;

UCLASS()
class RTS_API AUnitBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attack();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsPlayersUnit = false;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetActor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float UnitHealth = 20.0f;

	void IsSelected(bool bIsSelected);

	void SetAttackTargetActor(AActor* NewTargetActor);

protected:
	UPROPERTY(EditDefaultsOnly)
	UDecalComponent* DecalComponent = nullptr;

	UFUNCTION()
	void OnUnitClicked(AActor* Target, FKey ButtonPressed);

	ARTSPlayerControllerBase* RTSPlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FriendDecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FoeDecalMaterial = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetFriendFoeDecal();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly)
	float UnitAttackDamage = 5.0f;

	void UnitDeath();

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* DeathAnimation = nullptr;

	FTimerHandle DestroyDeadActorTimer;

	void DestroyDeadActor();

	UBlackboardComponent* ThisUnitBlackboard = nullptr;

	ARTSAIControllerBase* ThisUnitAIController = nullptr;

};
