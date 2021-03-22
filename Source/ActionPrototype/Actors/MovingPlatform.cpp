// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

#include <iterator>

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
	CurrentMode = InitialMode;

	if (StartPoint > PlatformPath->GetNumberOfSplinePoints())
	{
		StartPoint = PlatformPath->GetNumberOfSplinePoints();
		UE_LOG(
			   LogTemp,
			   Error,
			   TEXT(
				   "Illegal value. Strat point index can't be greater than the max number of points in spline.\nCheck the option in %s"
			   ),
			   *this->GetName()
			  );
	}

	if (WaitDuration > 0.f && StopoverPointsSet.Num() == 0)
	{
		PreviousPoint = StartPoint;
		TargetPoint = PreviousPoint + 1;
	}

	if (StopoverPointsSet.Num() > 0)
	{
		if (!StopoverPointsSet.Contains(0))
		{
			StopoverPointsSet.Add(0);
		}

		const int32 NumberOfPoints = PlatformPath->GetNumberOfSplinePoints();
		const int32 LastPoint = PlatformPath->IsClosedLoop() ? NumberOfPoints : NumberOfPoints - 1;

		if (!StopoverPointsSet.Contains(LastPoint))
		{
			StopoverPointsSet.Add(LastPoint);
		}


		for (int32 Point : StopoverPointsSet)
		{
			if (Point > LastPoint || Point < 0)
			{
				UE_LOG(
					   LogTemp,
					   Error,
					   TEXT(
						   "Illegal point index %d in StopoverPointsSet.\n Index can't be less than zero or more than the last point index in spline.\n Check actor %s"
					   ),
					   Point,
					   *this->GetName()
					  );
				StopoverPointsSet.Remove(Point);
			}
		}

		StopoverPointsArray = StopoverPointsSet.Array();
		StopoverPointsArray.Sort();
		StopoverPointsSet.Empty();
		PreviousPoint = StopoverPointsArray[0];
		TargetPoint = StopoverPointsArray[1];
	}
	Super::BeginPlay();
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

void AMovingPlatform::ProcessManualMode()
{
	PreviousPoint = TargetPoint;
	const int32 CurrentPointIndex = StopoverPointsArray.IndexOfByKey(PreviousPoint);
	int32 NextPointIndex = CurrentPointIndex + 1;

	if (NextPointIndex >= StopoverPointsArray.Num())
	{
		PreviousPoint = 0;
		NextPointIndex = PlatformPath->IsClosedLoop() ? 1 : 0;
	}

	TargetPoint = StopoverPointsArray[NextPointIndex];
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
		case EPlatformMode::Manual:
			ProcessManualMode();
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
