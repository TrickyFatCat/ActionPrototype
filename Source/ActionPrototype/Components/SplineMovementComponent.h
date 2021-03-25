// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "SplineMovementComponent.generated.h"

class USplineComponent;

UENUM()
enum class ESplineMovementMode : uint8
{
	OneWay,
	Loop,
	ReversedLoop,
	Manual
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMovement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArriveAtPoint, int32, SplinePointIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaitStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaitFinish);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONPROTOTYPE_API USplineMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USplineMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category="Spline Mover")
	FOnStartMovement OnStartMovement;
	UPROPERTY(BlueprintAssignable, Category="Spline Mover")
	FOnArriveAtPoint OnArriveAtPoint;
	UPROPERTY(BlueprintAssignable, Category="Spline Mover")
	FOnWaitStart OnWaitStart;
	UPROPERTY(BlueprintAssignable, Category="Spline Mover")
	FOnWaitFinish OnWaitFinish;

protected:
	UFUNCTION()
	void BroadcastOnWaitFinish() const;

private:
	UPROPERTY(BlueprintReadOnly, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	AActor* Owner{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	AActor* TargetActor{nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	USplineComponent* Spline{nullptr};
	int32 GetSplineLastPoint() const;
	bool HasOwnerAndSpline() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	ESplineMovementMode MovementMode{ESplineMovementMode::OneWay};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	bool bIsReversed{false};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Spline Mover",
		meta=(AllowPrivateAccess="true", ClampMin="0.0")
	)
	float WaitDuration{3.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline Mover", meta=(AllowPrivateAccess="true"))
	FTimerHandle WaitDurationHandle{};
	void StartWaitTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true", ClampMin="0"))
	int32 StartPointIndex{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true"))
	int32 PreviousPointIndex{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true"))
	int32 NextPointIndex{1};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true"))
	TArray<int32> PathPoints{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true"))
	bool bUseCustomPoints{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Path", meta=(AllowPrivateAccess="true", EditCondition="bUseCustomPoints"))
	TSet<int32> CustomPathPoints{};
	bool IsPointIndexOutOfBounds(const int32 PointIndex) const;
	void FillPathPoints();
	void CalculateStartPointIndex();
	void CalculateNextPointIndex();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Movement", meta=(AllowPrivateAccess="true"))
	float Speed{30.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Movement", meta=(AllowPrivateAccess="true"))
	bool bInheritPitch{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Movement", meta=(AllowPrivateAccess="true"))
	bool bInheritYaw{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline Mover|Movement", meta=(AllowPrivateAccess="true"))
	bool bInheritRoll{false};
	float GetCurrentSplinePosition(const float PathProgress) const;
	void SetLocationAlongSpline(const float PathProgress) const;
	void SetRotationAlongSpline(const float PathProgress) const;
	UFUNCTION(BlueprintCallable, Category="Spline Mover")
	void MoveAlongSpline(const float PathProgress);
	UFUNCTION(BlueprintCallable, Category="Spline Mover")
	void ContinueMoveAlongSpline();
};
