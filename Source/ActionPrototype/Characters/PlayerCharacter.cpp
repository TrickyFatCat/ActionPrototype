// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and adjust Spring Arm
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = DefaultArmLength;
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create and setup Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APlayerCharacter::SetCameraYawSensitivity(float NewSensitivity)
{
	if (NewSensitivity <= 0.f)
	{
		UE_LOG(LogInput, Error,
		       TEXT("Camera Yaw sensitivity can't be set less or equal 0. Sensitivity wasn't changed."));
		return false;
	}

	CameraYawSensitivity = NewSensitivity;

	return true;
}

bool APlayerCharacter::SetCameraPitchSensitivity(const float NewSensitivity)
{
	if (NewSensitivity <= 0.f)
	{
		UE_LOG(LogInput, Error,
		       TEXT("Camera Pitch sensitivity can't be set less or equal 0. Sensitivity wasn't changed."));
		return false;
	}

	CameraPitchSensitivity = NewSensitivity;

	return true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if (AxisValue != 0.f)
	{
		AddMovementInput(GetActorRightVector(), AxisValue);
	}
}

void APlayerCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue * CameraYawSensitivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue * CameraPitchSensitivity * GetWorld()->GetDeltaSeconds());
}
