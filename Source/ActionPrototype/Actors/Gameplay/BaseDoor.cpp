// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDoor.h"


ABaseDoor::ABaseDoor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseDoor::OpenDoor()
{
	if (CurrentState == EDoorState::Locked || CurrentState == EDoorState::Disabled || CurrentState == EDoorState::Opened
	)
	{
		return false;
	}

	if (bIsTransitionRevertible && CurrentState == EDoorState::Transition && TargetState == EDoorState::Closed)
	{
		RevertTransition();
		return true;
	}

	StartTransition();
	return true;
}

bool ABaseDoor::CloseDoor()
{
	if (CurrentState == EDoorState::Locked || CurrentState == EDoorState::Disabled || CurrentState == EDoorState::Closed
	)
	{
		return false;
	}

	if (bIsTransitionRevertible && CurrentState == EDoorState::Transition && TargetState == EDoorState::Opened)
	{
		RevertTransition();
		return true;
	}

	StartTransition();
	return true;
}

bool ABaseDoor::LockDoor()
{
	if (CurrentState == EDoorState::Locked || CurrentState != EDoorState::Closed)
	{
		return false;
	}

	ChangeStateTo(EDoorState::Locked);
	return true;
}

bool ABaseDoor::UnlockDoor()
{
	if (CurrentState != EDoorState::Locked)
	{
		return false;
	}

	CurrentState = EDoorState::Closed;
	OnUnlocked();
	return true;
}

bool ABaseDoor::DisableDoor()
{
	if (CurrentState != EDoorState::Closed || CurrentState != EDoorState::Opened)
	{
		return false;
	}

	ChangeStateTo(EDoorState::Disabled);
	return true;
}

bool ABaseDoor::EnableDoor(const EDoorState NewState)
{
	if (CurrentState != EDoorState::Disabled)
	{
		return false;
	}

	CurrentState = NewState;
	OnEnabled();
	return true;
}

void ABaseDoor::SetDoorLocation(
	UStaticMeshComponent* DoorMesh,
	const FVector InitialLocation,
	const FVector LocationOffset)
{
	if (DoorMesh == nullptr)
	{
		return;
	}
	
	FVector NewLocation = InitialLocation;
	NewLocation += LocationOffset;
	DoorMesh->SetWorldLocation(NewLocation);
}

void ABaseDoor::SetDoorRotation(
	UStaticMeshComponent* DoorMesh,
	const FRotator InitialRotation,
	const FRotator RotationOffset)
{
	if (DoorMesh == nullptr)
	{
		return;
	}

	FRotator NewRotation = InitialRotation;
	NewRotation += RotationOffset;
	DoorMesh->SetWorldRotation(NewRotation);
}

void ABaseDoor::ChangeStateTo(const EDoorState NewState)
{
	PreviousState = CurrentState;
	CurrentState = NewState;

	switch (CurrentState)
	{
		case EDoorState::Closed:
			OnClosed();
			OnDoorClosed.Broadcast();
			break;
		case EDoorState::Opened:
			OnOpened();
			OnDoorOpened.Broadcast();

			if (CloseDelay > 0.f)
			{
				FTimerDelegate CloseDelayDelegate;
				CloseDelayDelegate.BindUFunction(this, FName("ChangeStateTo"), EDoorState::Closed);
				GetWorld()->GetTimerManager().SetTimer(CloseDelayHandle, CloseDelayDelegate, CloseDelay, false);
			}
			break;
		case EDoorState::Locked:
			OnLocked();
			OnDoorLocked.Broadcast();
			break;
		case EDoorState::Transition:
			OnTransitionStarted();
			OnDoorTransitionStarted.Broadcast();
			break;
		case EDoorState::Disabled:
			OnDisabled();
			break;
		default:
			break;
	}
}

void ABaseDoor::StartTransition()
{
	if (CurrentState == EDoorState::Closed)
	{
		TargetState = EDoorState::Opened;

		if (GetWorld()->GetTimerManager().IsTimerActive(CloseDelayHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(CloseDelayHandle);
		}
	}
	else
	{
		TargetState = EDoorState::Closed;
	}

	TransitionDurationDelegate.Unbind();
	TransitionDurationDelegate.BindUFunction(this, FName("ChangeStateTo"), TargetState);
	GetWorld()->GetTimerManager().SetTimer(
										   TransitionDurationHandle,
										   TransitionDurationDelegate,
										   TransitionDuration,
										   false
										  );
	ChangeStateTo(EDoorState::Transition);
}

void ABaseDoor::RevertTransition()
{
	if (TargetState == EDoorState::Closed)
	{
		TargetState = EDoorState::Opened;
	}
	else
	{
		TargetState = EDoorState::Closed;
	}

	TransitionDurationDelegate.Unbind();
	TransitionDurationDelegate.BindUFunction(this, FName("ChangeStateTo"), TargetState);
	const float NewTransitionDuration = GetWorld()->GetTimerManager().GetTimerElapsed(TransitionDurationHandle);
	GetWorld()->GetTimerManager().ClearTimer(TransitionDurationHandle);
	GetWorld()->GetTimerManager().SetTimer(
										   TransitionDurationHandle,
										   TransitionDurationDelegate,
										   NewTransitionDuration,
										   false
										  );
	OnTransitionReverted();
	OnDoorTransitionReverted.Broadcast();
}
