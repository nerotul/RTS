// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCameraBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARTSCameraBase::ARTSCameraBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	SpringArm->TargetArmLength = 3000.0f;
	FRotator SpringArmRotation(-70, 0, 0);
	SpringArm->SetWorldRotation(SpringArmRotation);
	SpringArm->bDoCollisionTest = false;


}

void ARTSCameraBase::MoveLeft()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	ViewportScale = GEngine->GameViewport->GetDPIScale();
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float ScaledCursorLocationX = CursorLocationX * ViewportScale;
	float SensetiveViewportZone = ViewportSize.X * 0.05f;
	float NormalizedValue = UKismetMathLibrary::NormalizeToRange(ScaledCursorLocationX, 0, SensetiveViewportZone);
	float ClampedValue = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);

	DeltaLocation.X = (1 - ClampedValue) * 100.0f;

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveRight()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	ViewportScale = GEngine->GameViewport->GetDPIScale();
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float ScaledCursorLocationX = CursorLocationX * ViewportScale;
	float SensetiveViewportZone = ViewportSize.X * 0.95f;
	float NormalizedValue = UKismetMathLibrary::NormalizeToRange(ScaledCursorLocationX, SensetiveViewportZone, ViewportSize.X);
	float ClampedValue = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);

	DeltaLocation.X = ClampedValue * -100.0f;

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveUp()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	ViewportScale = GEngine->GameViewport->GetDPIScale();
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float ScaledCursorLocationY = CursorLocationY * ViewportScale;
	float SensetiveViewportZone = ViewportSize.Y * 0.05f;
	float NormalizedValue = UKismetMathLibrary::NormalizeToRange(ScaledCursorLocationY, 0, SensetiveViewportZone);
	float ClampedValue = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);

	DeltaLocation.Y = (1 - ClampedValue) * 100.0f;

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveDown()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	float ViewportScale = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	ViewportScale = GEngine->GameViewport->GetDPIScale();
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float ScaledCursorLocationY = CursorLocationY * ViewportScale;
	float SensetiveViewportZone = ViewportSize.Y * 0.95f;
	float NormalizedValue = UKismetMathLibrary::NormalizeToRange(ScaledCursorLocationY, SensetiveViewportZone, ViewportSize.Y);
	float ClampedValue = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);

	DeltaLocation.Y = ClampedValue * -100.0f;

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

// Called when the game starts or when spawned
void ARTSCameraBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(this);
}

// Called every frame
void ARTSCameraBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveLeft();
	MoveRight();
	MoveUp();
	MoveDown();

}

