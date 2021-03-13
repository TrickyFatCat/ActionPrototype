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
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::TriggerOverlapEnd);

	CurrentSwitchState = InitialSwitchState;
}

void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSwitchState == ESwitchState::Pressed && bRequirePressing)
	{
		OnSwitchPressing.Broadcast();
	}

	FString DebugMessage = FString::Printf(TEXT("Switch State: %s"), *UEnum::GetValueAsString(CurrentSwitchState));
	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, DebugMessage);
}

void AFloorSwitch::LockFloorSwitch()
{
	if (CurrentSwitchState == ESwitchState::Locked || CurrentSwitchState == ESwitchState::Transition)
	{
		return;
	}
	
	PreviousSwitchState = CurrentSwitchState;
	CurrentSwitchState = ESwitchState::Locked;
	OnSwitchLocked.Broadcast();
}

void AFloorSwitch::UnlockFloorSwitch()
{
	if (CurrentSwitchState != ESwitchState::Locked || CurrentSwitchState == ESwitchState::Transition)
	{
		return;
	}
	
	CurrentSwitchState = PreviousSwitchState;
	OnSwitchUnlocked.Broadcast();
}

void AFloorSwitch::DisableFloorSwitch()
{
	if (CurrentSwitchState == ESwitchState::Transition)
	{
		return;
	}
	
	TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.RemoveDynamic(this, &AFloorSwitch::TriggerOverlapEnd);
	TriggerVolume->SetGenerateOverlapEvents(false);
	TriggerVolume->SetCollisionResponseToChannels(ECR_Ignore);
	OnSwitchDisabled.Broadcast();
	SetActorTickEnabled(false);
}

void AFloorSwitch::TriggerOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                      bool bFromSweep,
                                                      const FHitResult& SweepResult)
{
	if (CurrentSwitchState == ESwitchState::Locked || CurrentSwitchState == ESwitchState::Transition)
	{
		return;
	}

	OnSwitchPressed.Broadcast();
	CurrentSwitchState = ESwitchState::Pressed;

	if (bTriggerOnce && !bIsPressedTemporary)
	{
		DisableFloorSwitch();
	}
}

void AFloorSwitch::TriggerOverlapEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CurrentSwitchState == ESwitchState::Transition)
	{
		return;
	}

	if (bIsPressedTemporary)
	{
		GetWorld()->GetTimerManager().SetTimer(PressedDurationTimer,this,  &AFloorSwitch::ChangeStateToNormal, PressedDuration, false); 
		return;
	}
	
	CurrentSwitchState = ESwitchState::Normal;
	UE_LOG(LogTemp, Warning, TEXT("%s stop being overlapped by %s"), *GetName(), *OtherActor->GetName());
}

void AFloorSwitch::ChangeStateToNormal()
{
	PreviousSwitchState = CurrentSwitchState;
	CurrentSwitchState = ESwitchState::Normal;
}