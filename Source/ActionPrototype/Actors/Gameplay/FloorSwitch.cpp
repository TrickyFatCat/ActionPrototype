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

	ActivationNumber = InitialActivationNumber;
	CurrentSwitchState = InitialSwitchState;

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapEnd);

	PressedTimerDelegate.BindUFunction(this, FName("ChangeStateTo"), EFloorSwitchState::Active);
	

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

void AFloorSwitch::UnlockFloorSwitch()
{
	if (CurrentSwitchState != EFloorSwitchState::Locked)
	{
		return;
	}

	if (PreviousSwitchState == EFloorSwitchState::Locked)
	{
		CurrentSwitchState = EFloorSwitchState::Active;
	}
	else
	{
		CurrentSwitchState = PreviousSwitchState;
	}

	OnSwitchUnlocked();
	OnFloorSwitchUnlocked.Broadcast();
}

void AFloorSwitch::DisableFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Disabled || CurrentSwitchState == EFloorSwitchState::Transition)
	{
		return;
	}

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
	OnFloorSwitchEnabled.Broadcast();
}

int32 AFloorSwitch::IncreaseActivationNumber(const int32 Amount)
{
	ActivationNumber += Amount;
	return ActivationNumber;
}

int32 AFloorSwitch::DecreaseActivationNumber(const int32 Amount)
{
	ActivationNumber = FMath::Max(ActivationNumber - Amount, 0);
	return ActivationNumber;
}

float AFloorSwitch::SetTransitionTime(const float NewTime)
{
	OnTransitionTimeChanged();
	return TransitionTime = FMath::Abs(NewTime);
}

float AFloorSwitch::CalculatePlayRate() const
{
	return 1 / TransitionTime;
}

void AFloorSwitch::UpdateButtonLocation(float OffsetX, float OffsetY, float OffsetZ)
{
	FVector NewLocation = InitialMeshLocation;
	NewLocation.X += OffsetX;
	NewLocation.Y += OffsetY;
	NewLocation.Z += OffsetZ;
	SwitchMesh->SetWorldLocation(NewLocation);
}

void AFloorSwitch::TriggerOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                      bool bFromSweep,
                                                      const FHitResult& SweepResult)
{
	if (CurrentSwitchState == EFloorSwitchState::Locked)
	{
		return;
	}

	if (CurrentSwitchState == EFloorSwitchState::Transition && bCanTransitionBeReverted)
	{
		RevertTransition();
		return;
	}

	if (bUseTimedTransition && CurrentSwitchState != EFloorSwitchState::Transition)
	{
		StartTransition();
		return;
	}

	ChangeStateTo(EFloorSwitchState::Pressed);
}

void AFloorSwitch::TriggerOverlapEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CurrentSwitchState == EFloorSwitchState::Transition && bCanTransitionBeReverted)
	{
		RevertTransition();
		return;
	}

	if (ActivationNumber > 0)
	{
		if (bIsPressedTemporary)
		{
			GetWorld()->GetTimerManager().SetTimer(PressedTimerHandle, PressedTimerDelegate, PressedDuration, false);
			return;
		}

		if (CurrentSwitchState != EFloorSwitchState::Transition)
		{
			StartTransition();
		}
		return;
	}

	ChangeStateTo(EFloorSwitchState::Active);
}

void AFloorSwitch::ChangeStateTo(const EFloorSwitchState NewState)
{
	PreviousSwitchState = CurrentSwitchState;
	CurrentSwitchState = NewState;

	switch (NewState)
	{
	case EFloorSwitchState::Active:
		OnSwitchActivated();
		OnFloorSwitchActive.Broadcast();
		break;
	case EFloorSwitchState::Pressed:
		OnSwitchPressed();
		OnFloorSwitchPressed.Broadcast();

		if (bIsActivationLimited)
		{
			DecreaseActivationNumber(1);

			if (ActivationNumber > 0)
			{
				return;
			}

			DisableFloorSwitch();
		}
		break;
	case EFloorSwitchState::Locked:
		OnSwitchLocked();
		OnFloorSwitchLocked.Broadcast();
		break;
	case EFloorSwitchState::Disabled:
		OnSwitchDisabled();
		OnFloorSwitchDisabled.Broadcast();
	case EFloorSwitchState::Transition:
		OnSwitchTransitionStarted();
		OnFloorSwitchTransitionStarted.Broadcast();
		break;
	default: ;
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
	GetWorld()->GetTimerManager().SetTimer(TransitionTimerHandle, TransitionTimerDelegate, TransitionTime,
	                                       false);
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

	// TODO add delay if IsPressedTemporary == true
	// TODO add different time for transition to Pressed and transition to Active
	const float NewTransitionTime = GetWorld()->GetTimerManager().GetTimerElapsed(TransitionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(TransitionTimerHandle);
	GetWorld()->GetTimerManager().
	            SetTimer(TransitionTimerHandle, TransitionTimerDelegate, NewTransitionTime, false);
	OnSwitchTransitionReverted();
	OnFloorSwitchTransitionReverted.Broadcast();
}
