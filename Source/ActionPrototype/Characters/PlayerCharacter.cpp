// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionPrototype/ActorComponents/BaseResourceComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and adjust Stamina component
	StaminaComponent = CreateDefaultSubobject<UBaseResourceComponent>(TEXT("Stamina Component"));

	// Create and adjust Spring Arm
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create and setup Camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	// Player character must not rotate with camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure player character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
}

void APlayerCharacter::BeginPlay()
{
	StaminaComponent->OnCurrentValueIncreased.AddDynamic(this, &APlayerCharacter::BroadcastStaminaIncreased);
	StaminaComponent->OnCurrentValueDecreased.AddDynamic(this, &APlayerCharacter::BroadcastStaminaDecreased);
	Super::BeginPlay();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APlayerCharacter::SetCameraYawSensitivity(const float NewSensitivity)
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

float APlayerCharacter::GetCurrentStamina() const
{
	return StaminaComponent->GetCurrentValue();
}

float APlayerCharacter::GetMaxStamina() const
{
	return StaminaComponent->GetMaxValue();
}

float APlayerCharacter::GetNormalisedStamina() const
{
	return StaminaComponent->GetNormalizedValue();
}

void APlayerCharacter::DecreaseStamina(const float Amount)
{
	StaminaComponent->DecreaseValue(Amount);
}

void APlayerCharacter::IncreaseStamina(const float Amount)
{
	StaminaComponent->IncreaseValue(Amount);
}

void APlayerCharacter::IncreaseMaxStamina(const float Amount)
{
	StaminaComponent->IncreaseMaxValue(Amount);
}

void APlayerCharacter::DecreaseMaxStamina(const float Amount)
{
	StaminaComponent->DecreaseMaxValue(Amount);
}

void APlayerCharacter::IncreaseCoins(const int32 Amount)
{
	Coins += Amount;
	OnCoinsIncreased.Broadcast(Amount, Coins);
}

void APlayerCharacter::DecreaseCoins(const int32 Amount)
{
	if (Coins == 0)
	{
		return;
	}

	Coins -= Amount;
	Coins = FMath::Max(Coins, 0);
	OnCoinsDecreased.Broadcast(Amount, Coins);
}

int32 APlayerCharacter::GetCoins() const
{
	return Coins;
}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator NewYawRotation(0.f, ControllerRotation.Yaw, 0.f);
		const FVector MoveDirection = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(MoveDirection, AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator NewYawRotation(0.f, ControllerRotation.Yaw, 0.f);
		const FVector MoveDirection = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(MoveDirection, AxisValue);
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

void APlayerCharacter::BroadcastStaminaIncreased(const float Amount, const float NewValue)
{
	OnStaminaIncreased.Broadcast(Amount, NewValue);
}

void APlayerCharacter::BroadcastStaminaDecreased(const float Amount, const float NewValue)
{
	OnStaminaDecreased.Broadcast(Amount, NewValue);
}
