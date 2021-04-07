// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionPrototype/ActorComponents/BaseResourceComponent.h"
#include "ActionPrototype/Actors/Weapon.h"
#include "ActionPrototype/Actors/Pickups/BasePickupItem.h"
#include "ActionPrototype/Interfaces/ReactToInteraction.h"
#include "Components/CapsuleComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh());

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

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::ProcessSprintAction);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::ProcessSprintAction);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
}

void APlayerCharacter::BeginPlay()
{
	StaminaDecreaseDeltaTime = StaminaDecreaseFrequency > 0.f ? 1.f / StaminaDecreaseFrequency : 0.f;

	Coins = 0;

	StaminaComponent->OnCurrentValueIncreased.AddDynamic(this, &APlayerCharacter::BroadcastStaminaIncreased);
	StaminaComponent->OnCurrentValueDecreased.AddDynamic(this, &APlayerCharacter::BroadcastStaminaDecreased);

	Super::BeginPlay();

	OnPlayerSpawned.Broadcast();

	if (GetMesh() != nullptr)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("hand_rSocket"));
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::AddToInteractionQueue);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::ActivatePickupEffect);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::RemoveFromInteractionQueue);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APlayerCharacter::SetCameraYawSensitivity(const float NewSensitivity)
{
	if (NewSensitivity <= 0.f)
	{
		UE_LOG(
			   LogInput,
			   Error,
			   TEXT("Camera Yaw sensitivity can't be set less or equal 0. Sensitivity wasn't changed.")
			  );
		return false;
	}

	CameraYawSensitivity = NewSensitivity;

	return true;
}

bool APlayerCharacter::SetCameraPitchSensitivity(const float NewSensitivity)
{
	if (NewSensitivity <= 0.f)
	{
		UE_LOG(
			   LogInput,
			   Error,
			   TEXT("Camera Pitch sensitivity can't be set less or equal 0. Sensitivity wasn't changed.")
			  );
		return false;
	}

	CameraPitchSensitivity = NewSensitivity;

	return true;
}

void APlayerCharacter::EquipWeapon(const TSubclassOf<AWeapon> NewWeapon) const
{
	if (Weapon->GetClass() == NewWeapon)
	{
		return;
	}

	Weapon->SetChildActorClass(NewWeapon);
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

EStaminaStatus APlayerCharacter::GetStaminaStatus() const
{
	const float CurrentStamina = GetNormalisedStamina();
	EStaminaStatus StaminaStatus;

	if (CurrentStamina > StaminaThresholds[0])
	{
		StaminaStatus = EStaminaStatus::High;
	}
	else if (CurrentStamina <= StaminaThresholds[0] && CurrentStamina > StaminaThresholds[1])
	{
		StaminaStatus = EStaminaStatus::Medium;
	}
	else
	{
		StaminaStatus = EStaminaStatus::Low;
	}

	return StaminaStatus;
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

void APlayerCharacter::SetSprintStaminaDecreaseFrequency(const float NewFrequency)
{
	if (NewFrequency <= 0.f)
	{
		StaminaDecreaseFrequency = 0.f;
		StaminaDecreaseDeltaTime = 0.f;
	}
	else
	{
		StaminaDecreaseFrequency = NewFrequency;
		StaminaDecreaseDeltaTime = 1.f / StaminaDecreaseFrequency;
	}
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

void APlayerCharacter::Interact()
{
	if (InteractionQueue.Num() == 0)
	{
		return;
	}

	IReactToInteraction::Execute_ProcessInteraction(InteractionQueue.Array()[0], this);
}

void APlayerCharacter::AddToInteractionQueue(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor->GetClass()->ImplementsInterface(UReactToInteraction::StaticClass()))
	{
		return;
	}

	InteractionQueue.Add(OtherActor);
}

void APlayerCharacter::RemoveFromInteractionQueue(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (InteractionQueue.Find(OtherActor))
	{
		InteractionQueue.Remove(OtherActor);
	}
}

void APlayerCharacter::ActivatePickupEffect(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ABasePickupItem* Pickup = Cast<ABasePickupItem>(OtherActor);

	if (Pickup && !Pickup->GetClass()->ImplementsInterface(UReactToInteraction::StaticClass()))
	{
		Pickup->ProcessPickup(this);
	}
}

void APlayerCharacter::BroadcastStaminaIncreased(const float Amount, const float NewValue)
{
	OnStaminaIncreased.Broadcast(Amount, NewValue);
}

void APlayerCharacter::BroadcastStaminaDecreased(const float Amount, const float NewValue)
{
	OnStaminaDecreased.Broadcast(Amount, NewValue);
}

void APlayerCharacter::DecreaseStaminaOnSprint()
{
	const FVector HorizontalVelocity = FVector(GetVelocity().X, GetVelocity().Y, 0.f);
	const float HorizontalSpeed = HorizontalVelocity.Size();

	if (HorizontalSpeed <= 0.f)
	{
		return;
	}

	DecreaseStamina(SprintStaminaCost);

	if (GetCurrentStamina() <= 0.f)
	{
		StopSprinting();
	}
}

void APlayerCharacter::StartDecreaseStamina()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (!TimerManager.IsTimerActive(DecreaseDeltaTimeHandle))
	{
		TimerManager.SetTimer(
							  DecreaseDeltaTimeHandle,
							  this,
							  &APlayerCharacter::DecreaseStaminaOnSprint,
							  StaminaDecreaseDeltaTime,
							  true
							 );
	}
}

void APlayerCharacter::StopDecreaseStamina()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(DecreaseDeltaTimeHandle))
	{
		TimerManager.ClearTimer(DecreaseDeltaTimeHandle);
	}
}

void APlayerCharacter::StartSprinting()
{
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		float& MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		MaxWalkSpeed *= SprintFactor;
		StartDecreaseStamina();
	}
}

void APlayerCharacter::StopSprinting()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		float& MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		MaxWalkSpeed /= SprintFactor;
		StopDecreaseStamina();
	}
}

void APlayerCharacter::ProcessSprintAction()
{
	bIsSprintPressed = !bIsSprintPressed;

	if (bIsSprintPressed)
	{
		StartSprinting();
	}
	else
	{
		StopSprinting();
	}
}
