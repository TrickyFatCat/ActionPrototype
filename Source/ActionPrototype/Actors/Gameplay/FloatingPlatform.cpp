// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"


#include "Components/SplineComponent.h"


// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	SetTargetSpline(ActorWithSpline);

	if (PathPoints.Num() == 0)
	{
		FillPathPoints();
	}

	CheckStartPointIndex();
	PreviousPointIndex = StartPointIndex;
	NextPointIndex = StartPointIndex;

	if (bAutoStart)
	{
		StartMovement();
	}
	
	Super::BeginPlay();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFloatingPlatform::SetTargetSpline(const AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return;
	}
	
	TargetSpline = Cast<USplineComponent>(TargetActor->GetComponentByClass(USplineComponent::StaticClass()));

	if (TargetSpline == nullptr)
	{
		UE_LOG(
			   LogTemp,
			   Error,
			   TEXT(
				   "Invalid TargetActor in %s. Chosen actor %s doesn't have a spline component. Choose another actor instead."
			   ),
			   *this->GetName(),
			   *TargetActor->GetName()
			  );
	}
}

void AFloatingPlatform::StartMovement()
{
	if (MovementMode == EFloatingPlatformMode::Manual && CurrentState != EFloatingPlatformState::Move)
	{
		return;
	}

	CurrentState = EFloatingPlatformState::Move;
	OnStartMovement();
	OnPlatformStartMovement.Broadcast();
}

void AFloatingPlatform::StopMovement()
{
	if (MovementMode == EFloatingPlatformMode::Manual && CurrentState != EFloatingPlatformState::Idle)
	{
		return;
	}
	CurrentState = EFloatingPlatformState::Idle;
	OnStopMovement();
	OnPlatformStopMovement.Broadcast();
}

void AFloatingPlatform::MoveToPoint(const int32 TargetPointIndex)
{
	if (!IsPointIndexOutOfBounds(TargetPointIndex))
	{
		NextPointIndex = TargetPointIndex;
		CalculateTravelTime();
		StartMovement();
	}
}

bool AFloatingPlatform::IsPointIndexOutOfBounds(const int32 PointIndex) const
{
	return PointIndex < 0 || PointIndex >= PathPoints.Num();
}

void AFloatingPlatform::SetSpeed(int32 NewSpeed)
{
	if (NewSpeed < 0)
	{
		NewSpeed *= -1;
	}

	Speed = NewSpeed;
	CalculateTravelTime();
}

int32 AFloatingPlatform::GetTargetSplineLastPoint() const
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return -1;
	}

	const int32 NumberOfPoints = TargetSpline->GetNumberOfSplinePoints();
	return TargetSpline->IsClosedLoop() ? NumberOfPoints : NumberOfPoints - 1;
}

float AFloatingPlatform::GetCurrentSplinePosition(const float PathProgress) const
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return -1.f;
	}

	const float Start = TargetSpline->GetDistanceAlongSplineAtSplinePoint(PathPoints[PreviousPointIndex]);
	const float Finish = TargetSpline->GetDistanceAlongSplineAtSplinePoint(PathPoints[NextPointIndex]);

	return FMath::Lerp(Start, Finish, PathProgress);
}

void AFloatingPlatform::SetLocationAlongSpline(const float PathProgress)
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return;
	}

	const float SplinePosition = GetCurrentSplinePosition(PathProgress);
	const FVector NewLocation = TargetSpline->GetLocationAtDistanceAlongSpline(
																			   SplinePosition,
																			   ESplineCoordinateSpace::World
																			  );
	SetActorLocation(NewLocation);
}

void AFloatingPlatform::SetRotationAlongSpline(const float PathProgress)
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return;
	}

	const float SplinePosition = GetCurrentSplinePosition(PathProgress);
	const FRotator CurrentRotation = GetActorRotation();
	const FRotator SplineRotation = TargetSpline->GetRotationAtDistanceAlongSpline(
		 SplinePosition,
		 ESplineCoordinateSpace::World
		);
	const float NewPitch = bInheritPitch ? SplineRotation.Pitch : CurrentRotation.Pitch;
	const float NewYaw = bInheritYaw ? SplineRotation.Yaw : CurrentRotation.Yaw;
	const float NewRoll = bInheritRoll ? SplineRotation.Roll : CurrentRotation.Roll;
	const FRotator NewRotation = FRotator(NewPitch, NewYaw, NewRoll);
	SetActorRotation(NewRotation);
}

void AFloatingPlatform::MoveAndRotateAlongSpline(const float PathProgress)
{
	SetLocationAlongSpline(PathProgress);
	SetRotationAlongSpline(PathProgress);
}

void AFloatingPlatform::ContinueMovementAlongSpline()
{
	if (CurrentState == EFloatingPlatformState::Idle)
	{
		return;
	}

	CalculatePointIndex();
	OnArrivedAtPoint(NextPointIndex);
	OnPlatformArrivedAtPoint.Broadcast(NextPointIndex);

	if (MovementMode == EFloatingPlatformMode::Manual)
	{
		return;
	}

	CalculateTravelTime();
	if (WaitDuration <= 0.f)
	{
		StartMovement();
	}
	else
	{
		StartWaitTimer();
	}
}

void AFloatingPlatform::StartWaitTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (WaitDuration > 0.f && !TimerManager.IsTimerActive(WaitTimerHandle))
	{
		CurrentState = EFloatingPlatformState::Wait;
		OnWaitStarted();
		OnPlatformWaitStarted.Broadcast();
		TimerManager.SetTimer(WaitTimerHandle, this, &AFloatingPlatform::FinishWaitTimer, WaitDuration, false);
	}
}

void AFloatingPlatform::FinishWaitTimer()
{
	CurrentState = EFloatingPlatformState::Move;
	OnWaitFinished();
	OnPlatformWaitFinished.Broadcast();
}

void AFloatingPlatform::CheckStartPointIndex()
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return;
	}

	if (StartPointIndex >= PathPoints.Num())
	{
		StartPointIndex = PathPoints.Num() - 1;
	}
	else if (StartPointIndex < 0)
	{
		StartPointIndex = 0;
	}
}

void AFloatingPlatform::CalculateNextPointIndex(const int32 PointIndex)
{
	NextPointIndex = bIsReversed ? PointIndex - 1 : PointIndex + 1;
}

void AFloatingPlatform::CalculatePointIndex()
{
	PreviousPointIndex = NextPointIndex;
	CalculateNextPointIndex(NextPointIndex);
	const bool bIsOutOfBounds = IsPointIndexOutOfBounds(NextPointIndex);

	switch (MovementMode)
	{
		case EFloatingPlatformMode::Loop:
			if (bIsOutOfBounds)
			{
				PreviousPointIndex = bIsReversed ? PathPoints.Num() - 1 : 0;
				CalculateNextPointIndex(PreviousPointIndex);
			}
			break;
		case EFloatingPlatformMode::ReversedLoop:
			if (bIsOutOfBounds)
			{
				bIsReversed = !bIsReversed;
				CalculateNextPointIndex(PreviousPointIndex);
			}
		default:
			break;
	}
}

void AFloatingPlatform::FillPathPoints()
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return;
	}

	if (bUseCustomPathPoints)
	{
		if (!CustomPathPoints.Contains(0))
		{
			CustomPathPoints.Add(0);
		}

		const int32 LastSplinePoint = GetTargetSplineLastPoint();
		if (!CustomPathPoints.Contains(LastSplinePoint))
		{
			CustomPathPoints.Add(LastSplinePoint);
		}

		TSet<int32> PathPointsSet{};

		for (const int32 Point : CustomPathPoints)
		{
			if (Point > 0 || Point <= LastSplinePoint)
			{
				PathPointsSet.Add(Point);
			}
		}

		PathPoints = PathPointsSet.Array();
		PathPoints.Sort();
		PathPointsSet.Empty();
		CustomPathPoints.Empty();

		for (const int32 Point : PathPoints)
		{
			CustomPathPoints.Add(Point);
		}
	}
	else
	{
		for (int PointIndex = 0; PointIndex < TargetSpline->GetNumberOfSplinePoints(); ++PointIndex)
		{
			PathPoints.Add(PointIndex);
		}
	}

	if (TargetSpline->IsClosedLoop())
	{
		PathPoints.Add(TargetSpline->GetNumberOfSplinePoints());
	}
}

void AFloatingPlatform::CalculateTravelTime()
{
	if (TargetSpline == nullptr)
	{
		PrintSplineNullError();
		return;
	}

	const float StartDistance = TargetSpline->GetDistanceAlongSplineAtSplinePoint(PathPoints[PreviousPointIndex]);
	const float FinishDistance = TargetSpline->GetDistanceAlongSplineAtSplinePoint(PathPoints[NextPointIndex]);
	const float DistanceBetweenPoints = FMath::Abs(FinishDistance - StartDistance);
	TravelTime = DistanceBetweenPoints / Speed;
}

void AFloatingPlatform::ProcessConstructionScript()
{
	if (ActorWithSpline == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't process construction in %s, ActorWithSpline is null."), *this->GetName());
		return;
	}
	
	PathPoints.Empty();
	SetTargetSpline(ActorWithSpline);
	FillPathPoints();
	CheckStartPointIndex();
	PreviousPointIndex = StartPointIndex;
	NextPointIndex = StartPointIndex;
	MoveAndRotateAlongSpline(0.0);
}

void AFloatingPlatform::PrintSplineNullError() const
{
	UE_LOG(LogTemp, Error, TEXT("TargetSpline is null in %s."), *this->GetName());
}
