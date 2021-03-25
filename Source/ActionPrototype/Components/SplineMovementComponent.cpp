// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"

// Sets default values for this component's properties
USplineMovementComponent::USplineMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USplineMovementComponent::BeginPlay()
{
	Owner = GetOwner();

	if (TargetActor != nullptr)
	{
		Spline = Cast<USplineComponent>(TargetActor->GetComponentByClass(USplineComponent::StaticClass()));
	}

	FillPathPoints();
	CalculateStartPointIndex();

	Super::BeginPlay();
}


// Called every frame
void USplineMovementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USplineMovementComponent::BroadcastOnWaitFinish() const
{
	OnWaitFinish.Broadcast();
}

int32 USplineMovementComponent::GetSplineLastPoint() const
{
	const int32 NumberOfPoints = Spline->GetNumberOfSplinePoints();
	return Spline->IsClosedLoop() ? NumberOfPoints : NumberOfPoints - 1;
}

bool USplineMovementComponent::HasOwnerAndSpline() const
{
	return Owner != nullptr && Spline != nullptr;
}

void USplineMovementComponent::StartWaitTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (WaitDuration > 0.f && !TimerManager.IsTimerActive(WaitDurationHandle))
	{
		OnWaitStart.Broadcast();
		TimerManager.SetTimer(
							  WaitDurationHandle,
							  this,
							  &USplineMovementComponent::BroadcastOnWaitFinish,
							  WaitDuration,
							  false
							 );
	}
}

bool USplineMovementComponent::IsPointIndexOutOfBounds(const int32 PointIndex) const
{
	return PointIndex < 0 || PointIndex >= PathPoints.Num();
}

void USplineMovementComponent::FillPathPoints()
{
	if (!HasOwnerAndSpline())
	{
		return;
	}

	if (bUseCustomPoints)
	{
		if (!CustomPathPoints.Contains(0))
		{
			CustomPathPoints.Add(0);
		}

		const int32 LastSplinePoint = GetSplineLastPoint();

		if (!CustomPathPoints.Contains(LastSplinePoint))
		{
			CustomPathPoints.Add(LastSplinePoint);
		}

		TSet<int32> UpdatedPathPoints{};

		for (const int32 Point : CustomPathPoints)
		{
			if (Point >= 0 || Point <= LastSplinePoint)
			{
				UpdatedPathPoints.Add(Point);
			}
		}

		PathPoints = UpdatedPathPoints.Array();
		PathPoints.Sort();
		UpdatedPathPoints.Empty();
	}
	else
	{
		for (int PointIndex = 0; PointIndex <= Spline->GetNumberOfSplinePoints(); ++PointIndex)
		{
			PathPoints.AddUnique(PointIndex);
		}
	}

	if (!Spline->IsClosedLoop())
	{
		PathPoints.Remove(Spline->GetNumberOfSplinePoints());
	}
}

void USplineMovementComponent::CalculateStartPointIndex()
{
	if (IsPointIndexOutOfBounds(StartPointIndex))
	{
		FString OwnerName{TEXT("NULL")};

		if (Owner != nullptr)
		{
			OwnerName = Owner->GetName();
		}

		UE_LOG(
			   LogTemp,
			   Error,
			   TEXT("Illigal StartPointIndex value %d. It can't be less than 0 or more than %d. Check actor %s."),
			   StartPointIndex,
			   PathPoints.Num() - 1,
			   *OwnerName
			  );

		StartPointIndex = 0;
	}

	if (WaitDuration > 0.f)
	{
		PreviousPointIndex = StartPointIndex;
		NextPointIndex = StartPointIndex;
	}
}

void USplineMovementComponent::CalculateNextPointIndex()
{
	PreviousPointIndex = NextPointIndex;
	NextPointIndex = bIsReversed ? NextPointIndex - 1 : NextPointIndex + 1;
	const bool bIsOutOfBounds = IsPointIndexOutOfBounds(NextPointIndex);

	switch (MovementMode)
	{
		case ESplineMovementMode::Loop:
			if (bIsOutOfBounds)
			{
				PreviousPointIndex = bIsReversed ? PathPoints.Num() - 1 : 0;
				NextPointIndex = bIsReversed ? PreviousPointIndex - 1 : PreviousPointIndex + 1;
			}
			break;
		case ESplineMovementMode::ReversedLoop:
			if (bIsOutOfBounds)
			{
				bIsReversed = !bIsReversed;
				NextPointIndex = bIsReversed ? 1 : PreviousPointIndex - 1;
			}
		default:
			break;
	}
}

float USplineMovementComponent::GetCurrentSplinePosition(const float PathProgress) const
{
	float Start;
	float Finish;

	if (WaitDuration <= 0.f)
	{
		Start = 0;
		Finish = Spline->GetSplineLength();
	}
	else
	{
		Start = Spline->GetDistanceAlongSplineAtSplinePoint(PathPoints[PreviousPointIndex]);
		Finish = Spline->GetDistanceAlongSplineAtSplinePoint(PathPoints[NextPointIndex]);
	}

	return FMath::Lerp(Start, Finish, PathProgress);
}

void USplineMovementComponent::SetLocationAlongSpline(const float PathProgress) const
{
	if (!HasOwnerAndSpline())
	{
		return;
	}

	const float SplinePosition = GetCurrentSplinePosition(PathProgress);
	const FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(SplinePosition, ESplineCoordinateSpace::World);
	Owner->SetActorLocation(NewLocation);
}

void USplineMovementComponent::SetRotationAlongSpline(const float PathProgress) const
{
	if (!HasOwnerAndSpline())
	{
		return;
	}

	const float SplinePosition = GetCurrentSplinePosition(PathProgress);
	const FRotator CurrentRotation = Owner->GetActorRotation();
	const FRotator SplineRotation = Spline->GetRotationAtDistanceAlongSpline(
																			 SplinePosition,
																			 ESplineCoordinateSpace::World
																			);
	const float NewPitch = bInheritPitch ? SplineRotation.Pitch : CurrentRotation.Pitch;
	const float NewYaw = bInheritYaw ? SplineRotation.Yaw : CurrentRotation.Yaw;
	const float NewRoll = bInheritRoll ? SplineRotation.Roll : CurrentRotation.Roll;
	const FRotator NewRotation = FRotator(NewPitch, NewYaw, NewRoll);
	Owner->SetActorRotation(NewRotation);
}

void USplineMovementComponent::MoveAlongSpline(const float PathProgress)
{
	if (!HasOwnerAndSpline())
	{
		return;
	}

	SetLocationAlongSpline(PathProgress);
	SetRotationAlongSpline(PathProgress);
}

void USplineMovementComponent::ContinueMoveAlongSpline()
{
	if (!HasOwnerAndSpline())
	{
		return;
	}
	
	if (WaitDuration > 0.f)
	{
		OnArriveAtPoint.Broadcast(PathPoints[NextPointIndex]);
		CalculateNextPointIndex();
	}
	else
	{
		OnStartMovement.Broadcast();
	}
	
	if (MovementMode == ESplineMovementMode::OneWay && IsPointIndexOutOfBounds(NextPointIndex))
	{
		return;
	}

	StartWaitTimer();
}
