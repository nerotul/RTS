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

	SpringArm->TargetArmLength = 5000.0f;
	FRotator SpringArmRotation(-70, 0, 0);
	SpringArm->SetWorldRotation(SpringArmRotation);
	SpringArm->bDoCollisionTest = false;


}

void ARTSCameraBase::MoveLeft()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float NormalizedCursorPosition = UKismetMathLibrary::NormalizeToRange(GetScaledCursorLocation(CursorLocationX), 0, GetViewportSensetiveZone(ViewportSize.X, 0.05f));
	float ClampedValue = FMath::Clamp(NormalizedCursorPosition, 0.0f, 1.0f);

	DeltaLocation.X = (1 - ClampedValue) * GetMovementSpeedScaledOnZoom();

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveRight()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float NormalizedCursorPosition = UKismetMathLibrary::NormalizeToRange(GetScaledCursorLocation(CursorLocationX), GetViewportSensetiveZone(ViewportSize.X, 0.95f), ViewportSize.X);
	float ClampedValue = FMath::Clamp(NormalizedCursorPosition, 0.0f, 1.0f);

	DeltaLocation.X = ClampedValue * -GetMovementSpeedScaledOnZoom();

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveUp()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float NormalizedCursorPosition = UKismetMathLibrary::NormalizeToRange(GetScaledCursorLocation(CursorLocationY), 0, GetViewportSensetiveZone(ViewportSize.Y, 0.05f));
	float ClampedValue = FMath::Clamp(NormalizedCursorPosition, 0.0f, 1.0f);

	DeltaLocation.Y = (1 - ClampedValue) * GetMovementSpeedScaledOnZoom();

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

void ARTSCameraBase::MoveDown()
{
	float CursorLocationX = 0.0f;
	float CursorLocationY = 0.0f;
	FVector2D ViewportSize;
	FVector DeltaLocation(0.0f, 0.0f, 0.0f);

	PlayerController->GetMousePosition(CursorLocationX, CursorLocationY);
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	float NormalizedCursorPosition = UKismetMathLibrary::NormalizeToRange(GetScaledCursorLocation(CursorLocationY), GetViewportSensetiveZone(ViewportSize.Y, 0.95f), ViewportSize.Y);
	float ClampedValue = FMath::Clamp(NormalizedCursorPosition, 0.0f, 1.0f);

	DeltaLocation.Y = ClampedValue * -GetMovementSpeedScaledOnZoom();

	RootSceneComponent->AddRelativeLocation(DeltaLocation);

}

float ARTSCameraBase::GetScaledCursorLocation(float CursorLocationToScale)
{
	float ViewportScale = 0.0f;
	ViewportScale = GEngine->GameViewport->GetDPIScale(); // Getting viewport scale

	return CursorLocationToScale * ViewportScale;
}

float ARTSCameraBase::GetViewportSensetiveZone(float ViewportAxis, float ZonePercantage)
{
	return ViewportAxis * ZonePercantage;  // Setting which viewport zone percentage will move camera when cursor is in it
}

float ARTSCameraBase::GetMovementSpeedScaledOnZoom()
{
	float SpringArmLenghNormalized = UKismetMathLibrary::NormalizeToRange(SpringArm->TargetArmLength, 1000.0f, 5000.0f);
	return (SpringArmLenghNormalized * 40.0f + 60.0f); // Creating dependency between zoom scale and camera movement speed

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

	// For some reason right and down movements react on cursor earlier than others in editor, while it works fine in build
	MoveLeft();
	MoveRight();
	MoveUp();
	MoveDown();

}

