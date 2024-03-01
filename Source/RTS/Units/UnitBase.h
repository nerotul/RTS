// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "UnitBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDead, bool, bIsPlayersUnit);

class ARTSPlayerControllerBase;
class UBlackboardComponent;
class ARTSAIControllerBase;
class URTSAbilitySystemComponent;
class UAbilitySystemComponent;
class URTSAttributeSet;
class UGameplayEffect;

UCLASS()
class RTS_API AUnitBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnitBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Attack();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsPlayersUnit = false;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetActor = nullptr;

	void IsSelected(bool bIsSelected);

	void SetAttackTargetActor(AActor* NewTargetActor);

	UPROPERTY(BlueprintReadOnly)
	bool bIsAlive = true;

	UPROPERTY(BlueprintAssignable)
	FOnUnitDead OnUnitDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	URTSAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	const URTSAttributeSet* AttributeSet = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAbility")
	TSubclassOf<UGameplayEffect> DamageEffect;

	void UnitDeath();

	ARTSAIControllerBase* ThisUnitAIController = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly)
	UDecalComponent* DecalComponent = nullptr;

	UFUNCTION()
	virtual void OnUnitClicked(AActor* Target, FKey ButtonPressed);

	ARTSPlayerControllerBase* RTSPlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FriendDecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FoeDecalMaterial = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetFriendFoeDecal();

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* DeathAnimation = nullptr;

	FTimerHandle DestroyDeadActorTimer;

	void DestroyDeadActor();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};
