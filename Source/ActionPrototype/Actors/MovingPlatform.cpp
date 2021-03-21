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
	FillPointsPositions();
	CurrentMode = InitialMode;
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMovingPlatform::ClearPassedPoints()
{
	PassedPoints.Empty();
}

void AMovingPlatform::SetMovingPlatformMode(const EPlatformMode NewMode)
{
	if (NewMode == CurrentMode)
	{
		return;
	}

	CurrentMode = NewMode;
}

void AMovingPlatform::FillPointsPositions()
{
	const float SplineLength = PlatformPath->GetSplineLength();
	for (int i = 0; i < PlatformPath->GetNumberOfSplinePoints(); ++i)
	{
		PointsPositions.Add(PlatformPath->GetDistanceAlongSplineAtSplinePoint(i) / SplineLength);
	}
}

void AMovingPlatform::CheckPointsOnPath(const float PathProgress)
{
	const int32 ArraySize = PointsPositions.Num();

	if (ArraySize == 0)
	{
		return;
	}

	for (int i = 0; i < ArraySize; ++i)
	{
		if (PassedPoints.Contains(i))
		{
			continue;
		}

		if (bIsReversed)
		{
			if (PathProgress <= PointsPositions[i])
			{
				AddPointToPassedPoints(i);
			}
		}
		else
		{
			if (PathProgress >= PointsPositions[i])
			{
				AddPointToPassedPoints(i);
			}
		}
	}
}

void AMovingPlatform::AddPointToPassedPoints(const int32 PointIndex)
{
	PassedPoints.Add(PointIndex);
	OnPointPassed(PointIndex);
	StartWaitTimer();
}

float AMovingPlatform::GetCurrentPlatformPosition(const float PathProgress) const
{
	return FMath::Lerp(0.f, PlatformPath->GetSplineLength(), PathProgress);
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

void AMovingPlatform::ProcessPlatformMovement(const float PathProgress)
{
	CheckPointsOnPath(PathProgress);
	MoveAlongSpline(PathProgress);
	RotateAlongSpline(PathProgress);
}

void AMovingPlatform::StartWaitTimer()
{
	if (WaitDuration > 0.f && !GetWorld()->GetTimerManager().IsTimerActive(WaitDurationHandle))
	{
		OnPlatformWaitStart();
		GetWorld()->GetTimerManager().SetTimer(
                                               WaitDurationHandle,
                                               this,
                                               &AMovingPlatform::OnPlatformWaitFinish,
                                               WaitDuration,
                                               false
                                              );
	}
}
