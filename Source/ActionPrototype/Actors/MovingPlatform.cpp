// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/SplineComponent.h"
#include "Containers/Array.h"


// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformPath = CreateDefaultSubobject<USplineComponent>(TEXT("Platform Path"));
	RootComponent = PlatformPath;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	PlatformMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	CurrentMode = InitialMode;

	if (WaitDuration > 0)
	{
		PreviousPoint = StartPoint;
		TargetPoint = PreviousPoint + 1;
	}
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMovingPlatform::SetMovingPlatformMode(const EPlatformMode NewMode)
{
	if (NewMode == CurrentMode)
	{
		return;
	}

	CurrentMode = NewMode;
}

bool AMovingPlatform::IsTargetPointOutOfBounds() const
{
	if (bIsReversed)
	{
		return TargetPoint < 0;
	}

	if (PlatformPath->IsClosedLoop())
	{
		return TargetPoint > PlatformPath->GetNumberOfSplinePoints();
	}

	return TargetPoint >= PlatformPath->GetNumberOfSplinePoints();
}

void AMovingPlatform::ProcessOneWayMode()
{
	PreviousPoint = TargetPoint;
	TargetPoint++;

	if (IsTargetPointOutOfBounds())
	{
		return;
	}

	ContinueMovement();
}

void AMovingPlatform::ProcessLoopMode()
{
	PreviousPoint = TargetPoint;
	TargetPoint++;

	if (IsTargetPointOutOfBounds())
	{
		PreviousPoint = 0;
		TargetPoint = 1;
	}

	ContinueMovement();
}

void AMovingPlatform::ProcessReverseLoopMode()
{
	PreviousPoint = TargetPoint;

	if (bIsReversed)
	{
		TargetPoint--;

		if (IsTargetPointOutOfBounds())
		{
			bIsReversed = false;
			TargetPoint = 1;
		}
	}
	else
	{
		TargetPoint++;

		if (IsTargetPointOutOfBounds())
		{
			bIsReversed = true;
			TargetPoint = PreviousPoint - 1;
		}
	}

	ContinueMovement();
}

void AMovingPlatform::ContinueMovement()
{
	StartWaitTimer();

	if (WaitDuration <= 0.f)
	{
		OnStartMovement();
	}
}

void AMovingPlatform::CalculateTargetPoint()
{
	if (WaitDuration <= 0.f)
	{
		return;
	}

	OnArrivedInPoint(TargetPoint);
	
	switch (CurrentMode)
	{
		case EPlatformMode::OneWay:
			ProcessOneWayMode();
			break;
		case EPlatformMode::Loop:
			ProcessLoopMode();
			break;
		case EPlatformMode::ReverseLoop:
			ProcessReverseLoopMode();
			break;
		default:
			break;
	}
}

float AMovingPlatform::GetCurrentPlatformPosition(const float PathProgress) const
{
	float Start;
	float End;

	if (WaitDuration <= 0.f)
	{
		Start = 0;
		End = PlatformPath->GetSplineLength();
	}
	else
	{
		Start = PlatformPath->GetDistanceAlongSplineAtSplinePoint(PreviousPoint);
		End = PlatformPath->GetDistanceAlongSplineAtSplinePoint(TargetPoint);
	}

	return FMath::Lerp(Start, End, PathProgress);
}

void AMovingPlatform::MoveAlongSpline(const float PathProgress) const
{
	const float CurrentPosition = GetCurrentPlatformPosition(PathProgress);
	const FVector NewLocation = PlatformPath->GetLocationAtDistanceAlongSpline(
																			   CurrentPosition,
																			   ESplineCoordinateSpace::World
																			  );
	PlatformMesh->SetWorldLocation(NewLocation);
}

void AMovingPlatform::RotateAlongSpline(const float PathProgress) const
{
	const float CurrentPosition = GetCurrentPlatformPosition(PathProgress);
	const FRotator CurrentRotation = PlatformMesh->GetComponentRotation();
	const FRotator SplineRotation = PlatformPath->GetRotationAtDistanceAlongSpline(
		 CurrentPosition,
		 ESplineCoordinateSpace::World
		);
	const float NewPitch = bInheritPitch ? SplineRotation.Pitch : CurrentRotation.Pitch;
	const float NewYaw = bInheritYaw ? SplineRotation.Yaw : CurrentRotation.Pitch;
	const float NewRoll = bInheritRoll ? SplineRotation.Roll : CurrentRotation.Roll;
	const FRotator NewRotation = FRotator(NewPitch, NewYaw, NewRoll);
	PlatformMesh->SetWorldRotation(NewRotation);
}

void AMovingPlatform::MovePlatform(const float PathProgress)
{
	MoveAlongSpline(PathProgress);
	RotateAlongSpline(PathProgress);
}

void AMovingPlatform::StartWaitTimer()
{
	if (WaitDuration > 0.f && !GetWorld()->GetTimerManager().IsTimerActive(WaitDurationHandle))
	{
		OnWaitStart();
		GetWorld()->GetTimerManager().SetTimer(
											   WaitDurationHandle,
											   this,
											   &AMovingPlatform::OnWaitFinish,
											   WaitDuration,
											   false
											  );
	}
}
