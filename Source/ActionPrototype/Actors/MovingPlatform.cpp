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
		TargetPoint = bIsReversed ? PreviousPoint - 1 : PreviousPoint + 1;
	}

	if (StopoverPointsSet.Num() > 0)
	{
		if (!StopoverPointsSet.Contains(0))
		{
			StopoverPointsSet.Add(0);
		}

		const int32 LastPoint = GetLastPoint();

		if (!StopoverPointsSet.Contains(LastPoint))
		{
			StopoverPointsSet.Add(LastPoint);
		}

		TSet<int32> ClearSet;

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
			}
			else
			{
				ClearSet.Add(Point);
			}
		}

		StopoverPointsArray = ClearSet.Array();
		StopoverPointsArray.Sort();
		StopoverPointsSet.Empty();
		ClearSet.Empty();
		PreviousPoint = bIsReversed ? StopoverPointsArray[GetLastIndex()] : StopoverPointsArray[0];
		TargetPoint = bIsReversed ? StopoverPointsArray[GetLastIndex() - 1] : StopoverPointsArray[1];
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

int32 AMovingPlatform::GetLastIndex() const
{
	return StopoverPointsArray.Num() - 1;
}

int32 AMovingPlatform::GetLastPoint() const
{
	const int32 NumberOfPoints = PlatformPath->GetNumberOfSplinePoints();
	return PlatformPath->IsClosedLoop() ? NumberOfPoints : NumberOfPoints - 1;
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


void AMovingPlatform::ContinueMovement()
{
	StartWaitTimer();

	if (WaitDuration <= 0.f)
	{
		OnStartMovement();
	}
}

void AMovingPlatform::CalculateNextPoint()
{
	TargetPoint = bIsReversed ? TargetPoint - 1 : TargetPoint + 1;
	const bool bIsOutOfBounds =	IsTargetPointOutOfBounds();

	switch (CurrentMode)
	{
		case EPlatformMode::OneWay:
			if (bIsOutOfBounds)
			{
				return;
			}

			ContinueMovement();
			break;
		case EPlatformMode::Loop:
			if (bIsOutOfBounds)
			{
				PreviousPoint = bIsReversed ? GetLastPoint() : 0;
				TargetPoint = bIsReversed ? PreviousPoint - 1 : PreviousPoint + 1;
			}

			ContinueMovement();
			break;
		case EPlatformMode::ReverseLoop:
			if (bIsReversed)
			{
				if (bIsOutOfBounds)
				{
					bIsReversed = false;
					TargetPoint = 1;
				}
			}
			else
			{
				if (bIsOutOfBounds)
				{
					bIsReversed = true;
					TargetPoint = PreviousPoint - 1;
				}
			}

			ContinueMovement();
			break;
		default:
			break;
	}
}

void AMovingPlatform::CalculateNextStopover()
{
	const int32 CurrentPointIndex = StopoverPointsArray.IndexOfByKey(PreviousPoint);
	int32 NextPointIndex = bIsReversed ? CurrentPointIndex - 1 : CurrentPointIndex + 1;
	const bool bIsOutOfBounds = NextPointIndex >= StopoverPointsArray.Num() || NextPointIndex < 0;

	switch (CurrentMode)
	{
		case EPlatformMode::OneWay:
			if (bIsOutOfBounds)
			{
				return;
			}

			TargetPoint = StopoverPointsArray[NextPointIndex];
			ContinueMovement();
			break;
		case EPlatformMode::Loop:
			if (bIsOutOfBounds)
			{
				const int32 LastIndex = GetLastIndex();
				PreviousPoint = bIsReversed ? StopoverPointsArray[LastIndex] : 0;
				NextPointIndex = bIsReversed ? LastIndex - 1 : 1;
			}

			TargetPoint = StopoverPointsArray[NextPointIndex];
			ContinueMovement();
			break;
		case EPlatformMode::ReverseLoop:
			if (bIsReversed)
			{
				if (bIsOutOfBounds)
				{
					bIsReversed = false;
					NextPointIndex = 1;
				}
			}
			else
			{
				if (bIsOutOfBounds)
				{
					bIsReversed = true;
					NextPointIndex = GetLastIndex() - 1;
				}
			}

			TargetPoint = StopoverPointsArray[NextPointIndex];
			ContinueMovement();
			break;
		default:
			break;
	}
}

void AMovingPlatform::ChangeTargetPoint()
{
	if (WaitDuration <= 0.f)
	{
		return;
	}

	OnArrivedInPoint(TargetPoint);
	PreviousPoint = TargetPoint;

	if (StopoverPointsArray.Num() <= 0)
	{
		CalculateNextPoint();
	}
	else
	{
		CalculateNextStopover();
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
