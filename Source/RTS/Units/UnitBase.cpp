// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "Components/DecalComponent.h"
#include "RTS/Player/RTSPlayerControllerBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "RTS/AI/RTSAIControllerBase.h"
#include "Components/CapsuleComponent.h"
#include "RTS/GAS/RTSAbilitySystemComponent.h"
#include "RTS/GAS/RTSAttributeSet.h"

// Sets default values
AUnitBase::AUnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	AbilitySystemComponent = CreateDefaultSubobject<URTSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<URTSAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	OnClicked.AddUniqueDynamic(this, &AUnitBase::OnUnitClicked);

	RTSPlayerController = Cast<ARTSPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
	ThisUnitAIController = Cast<ARTSAIControllerBase>(GetController());
}

// Called every frame
void AUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AUnitBase::Attack()
{
	TSubclassOf <UDamageType> DamageType;
	UGameplayStatics::ApplyDamage(TargetActor, UnitAttackDamage, GetController(), this, DamageType);
}

void AUnitBase::IsSelected(bool bIsSelected)
{
	DecalComponent->SetVisibility(bIsSelected);
}

void AUnitBase::OnUnitClicked(AActor* Target, FKey ButtonPressed)
{
	if (ButtonPressed.GetFName() == FName("LeftMouseButton") && bIsPlayersUnit == true && bIsAlive == true)
	{
		if (DecalComponent->IsVisible() == false)
		{
			RTSPlayerController->AddUnitToSelection(this);
		}
		else if (DecalComponent->IsVisible() == true)
		{
			RTSPlayerController->RemoveUnitFromSelection(this);
		}

	}
	else if (ButtonPressed.GetFName() == FName("RightMouseButton") && bIsPlayersUnit == false && bIsAlive == true)
	{
		for (AUnitBase* Unit : RTSPlayerController->UnitSelection)
		{
			Unit->SetAttackTargetActor(this);
		}

	}
}

void AUnitBase::SetAttackTargetActor(AActor* NewTargetActor)
{
	if (ThisUnitAIController->UnitBlackboard != nullptr)
	{
		TargetActor = NewTargetActor;
		ThisUnitAIController->UnitBlackboard->SetValueAsObject(FName("AttackTargetActor"), NewTargetActor);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Blackboard not found!"));
	}

}

// Called in BP construction script to visualize friend/foe status in editor
void AUnitBase::SetFriendFoeDecal()
{
	if (bIsPlayersUnit == true)
	{
		DecalComponent->SetMaterial(0, FriendDecalMaterial);
		DecalComponent->SetVisibility(false);
	}
	else
	{
		DecalComponent->SetMaterial(0, FoeDecalMaterial);
		DecalComponent->SetVisibility(true);
	}
}

float AUnitBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAlive == true)
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		UnitCurrentHealth -= DamageAmount;
		if (UnitCurrentHealth <= 0)
		{
			UnitDeath();
		}

		// Reporting damage event to the AI Perception to start retaliation attack
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator, DamageAmount, DamageCauser->GetActorLocation(), this->GetActorLocation());

	}

	return DamageAmount;
}

void AUnitBase::UnitDeath()
{
	if (IsValid(GetController()))
	{
		OnUnitDead.Broadcast(bIsPlayersUnit);
		GetController()->Destroy();
		bIsAlive = false;
		GetWorldTimerManager().SetTimer(DestroyDeadActorTimer, this, &AUnitBase::DestroyDeadActor, 1.0f, false, 5.0f);

		// If unit is friendly and was selected
		if (bIsPlayersUnit == true && DecalComponent->IsVisible())
		{
			RTSPlayerController->RemoveUnitFromSelection(this);
		}

		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->PlayAnimation(DeathAnimation, false);

}

void AUnitBase::DestroyDeadActor()
{
	Destroy();
}

UAbilitySystemComponent* AUnitBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUnitBase::InitAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();

		EffectContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecificationHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContextHandle);

		if (SpecificationHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecificationHandle.Data.Get());
		}
	}
}

void AUnitBase::InitDefaultAbilities()
{
}

void AUnitBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitAttributes();
}
