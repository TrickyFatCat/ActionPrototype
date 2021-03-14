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
}

void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSwitchState == EFloorSwitchState::Pressed && bGeneratePressingEvent)
	{
		OnSwitchPressing.Broadcast();
	}
}

void AFloorSwitch::LockFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Locked)
	{
		return;
	}
	
	ChangeStateTo(EFloorSwitchState::Locked);
	OnSwitchLocked.Broadcast();
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
	
	OnSwitchUnlocked.Broadcast();
}

void AFloorSwitch::DisableFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Disabled)
	{
		return;
	}

	ChangeStateTo(EFloorSwitchState::Disabled);
	TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapEnd);
	TriggerVolume->SetGenerateOverlapEvents(false);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	SetActorTickEnabled(false);
	OnSwitchDisabled.Broadcast();
}

void AFloorSwitch::EnableFloorSwitch()
{
	if (CurrentSwitchState == EFloorSwitchState::Disabled)
	{
		return;
	}
	
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapEnd);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetActorTickEnabled(true);
	ChangeStateTo(EFloorSwitchState::Active);
	OnSwitchEnabled.Broadcast();
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

void AFloorSwitch::TriggerOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                      bool bFromSweep,
                                                      const FHitResult& SweepResult)
{
	if (CurrentSwitchState == EFloorSwitchState::Locked)
	{
		return;
	}

	OnSwitchPressed.Broadcast();
	CurrentSwitchState = EFloorSwitchState::Pressed;

	if (bIsActivationLimited)
	{
		DecreaseActivationNumber(1);
		
		if (ActivationNumber > 0)
		{
			return;
		}
		
		DisableFloorSwitch();
	}
}

void AFloorSwitch::TriggerOverlapEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsPressedTemporary && ActivationNumber > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PressedDurationTimer, PressedTimerDelegate, PressedDuration, false); 
		return;
	}
	
	CurrentSwitchState = EFloorSwitchState::Active;
}

void AFloorSwitch::ChangeStateTo(const EFloorSwitchState NewState)
{
	PreviousSwitchState = CurrentSwitchState;
	CurrentSwitchState = NewState;
}