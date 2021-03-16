// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AFloorSwitch::AFloorSwitch()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	RootComponent = TriggerVolume;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldStatic);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch Mesh"));
	SwitchMesh->SetupAttachment(RootComponent);
}

void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	PressesNumber = InitialPressesNumber;
	CurrentSwitchState = InitialSwitchState;

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapEnd);

	InitialMeshLocation = SwitchMesh->GetComponentLocation();
}


void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFloorSwitch::LockFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Locked || CurrentSwitchState == EFloorSwitchState::Transition)
	{
		return;
	}

	ChangeStateTo(EFloorSwitchState::Locked);
}

void AFloorSwitch::UnlockFloorSwitch(const EFloorSwitchState NewState)
{
	if (CurrentSwitchState != EFloorSwitchState::Locked)
	{
		return;
	}

	ChangeStateTo(NewState);
	OnSwitchUnlocked();
}

void AFloorSwitch::DisableFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Disabled || CurrentSwitchState == EFloorSwitchState::Transition)
	{
		return;
	}

	bActorIsInTrigger = false;
	TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapEnd);
	TriggerVolume->SetGenerateOverlapEvents(false);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	SetActorTickEnabled(false);
	ChangeStateTo(EFloorSwitchState::Disabled);
}

void AFloorSwitch::EnableFloorSwitch()
{
	if (CurrentSwitchState != EFloorSwitchState::Disabled)
	{
		return;
	}

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapEnd);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetActorTickEnabled(true);
	CurrentSwitchState = EFloorSwitchState::Active;
	OnSwitchEnabled();
}

int32 AFloorSwitch::IncreasePressesNumber(const int32 Amount)
{
	PressesNumber += Amount;
	return PressesNumber;
}

int32 AFloorSwitch::DecreasePressesNumber(const int32 Amount)
{
	PressesNumber = FMath::Max(PressesNumber - Amount, 0);
	return PressesNumber;
}

float AFloorSwitch::SetTransitionTime(const float NewTime)
{
	OnTransitionTimeChanged();
	return TransitionTime = FMath::Abs(NewTime);
}

void AFloorSwitch::UpdateButtonLocation(const float OffsetX, const float OffsetY, const float OffsetZ) const
{
	FVector NewLocation = InitialMeshLocation;
	NewLocation.X += OffsetX;
	NewLocation.Y += OffsetY;
	NewLocation.Z += OffsetZ;
	SwitchMesh->SetWorldLocation(NewLocation);
}

void AFloorSwitch::TriggerOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	)
{
	bActorIsInTrigger = true;

	if (CurrentSwitchState == EFloorSwitchState::Locked)
	{
		return;
	}

	if (CurrentSwitchState == EFloorSwitchState::Transition && bIsTransitionRevertible)
	{
		RevertTransition();
		return;
	}

	if (CurrentSwitchState != EFloorSwitchState::Transition)
	{
		StartTransition();
	}
}

void AFloorSwitch::TriggerOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	)
{
	bActorIsInTrigger = false;

	if (CurrentSwitchState == EFloorSwitchState::Transition && bIsTransitionRevertible)
	{
		RevertTransition();
		return;
	}

	if (PressesNumber <= 0)
	{
		return;
	}

	if (PressedDuration > 0.f && CurrentSwitchState == EFloorSwitchState::Pressed)
	{
		SetPressedTimer();
		return;
	}

	if (CurrentSwitchState != EFloorSwitchState::Transition && PressedDuration <= 0.f)
	{
		StartTransition();
	}
}

void AFloorSwitch::ChangeStateTo(const EFloorSwitchState NewState)
{
	PreviousSwitchState = CurrentSwitchState;
	CurrentSwitchState = NewState;
	OnStateChanged();

	switch (NewState)
	{
		case EFloorSwitchState::Active:
			OnSwitchActivated();
			break;
		case EFloorSwitchState::Pressed:
			OnSwitchPressed();
			OnFloorSwitchPressed.Broadcast();

			if (bLimitedPresses)
			{
				DecreasePressesNumber(1);

				if (PressesNumber > 0)
				{
					return;
				}

				// Consider to give an ability to choose state on PressesNumber < 0
				ChangeStateTo(EFloorSwitchState::Locked);
				return;
			}

			if (PressedDuration > 0.f && !bActorIsInTrigger)
			{
				SetPressedTimer();
				return;
			}

			if (!bActorIsInTrigger)
			{
				StartTransition();
			}
			break;
		case EFloorSwitchState::Locked:
			OnSwitchLocked();
			OnFloorSwitchLocked.Broadcast();
			break;
		case EFloorSwitchState::Disabled:
			OnSwitchDisabled();
			break;
		case EFloorSwitchState::Transition:
			OnSwitchTransitionStarted();
			OnFloorSwitchTransitionStarted.Broadcast();
			break;
		default:
			break;
	}
}

void AFloorSwitch::StartTransition()
{
	if (TargetSwitchState == EFloorSwitchState::Active)
	{
		TargetSwitchState = EFloorSwitchState::Pressed;
	}
	else
	{
		TargetSwitchState = EFloorSwitchState::Active;
	}
	ChangeStateTo(EFloorSwitchState::Transition);
	TransitionTimerDelegate.BindUFunction(this, FName("ChangeStateTo"), TargetSwitchState);
	GetWorld()->GetTimerManager().SetTimer(
										   TransitionTimerHandle,
										   TransitionTimerDelegate,
										   TransitionTime,
										   false
										  );
}

void AFloorSwitch::RevertTransition()
{
	if (TargetSwitchState == EFloorSwitchState::Active)
	{
		TargetSwitchState = EFloorSwitchState::Pressed;
	}
	else
	{
		TargetSwitchState = EFloorSwitchState::Active;
	}
	TransitionTimerDelegate.BindUFunction(this, FName("ChangeStateTo"), TargetSwitchState);

	// TODO add different time for transition to Pressed and transition to Active
	const float NewTransitionTime = GetWorld()->GetTimerManager().GetTimerElapsed(TransitionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(TransitionTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TransitionTimerHandle, TransitionTimerDelegate, NewTransitionTime, false);
	OnSwitchTransitionReverted();
	OnFloorSwitchTransitionReverted.Broadcast();
}

void AFloorSwitch::SetPressedTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
										   PressedTimerHandle,
										   this,
										   &AFloorSwitch::StartTransition,
										   PressedDuration,
										   false
										  );
}
