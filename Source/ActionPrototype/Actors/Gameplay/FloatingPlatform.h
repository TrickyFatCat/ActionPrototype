// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

class USplineComponent;

UENUM(BlueprintType)
enum class EFloatingPlatformMode : uint8
{
	Manual,
	Loop,
	ReversedLoop
};

UENUM(BlueprintType)
enum class EFloatingPlatformState : uint8
{
	Idle,
	Move,
	Wait
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlatformStartMovement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlatformStopMovement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlatformArrivedAtPoint, int32, PointIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlatformWaitStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlatformWaitFinished);

UCLASS()
class ACTIONPROTOTYPE_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AFloatingPlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Sets TargetSpline value if the given actor has USplineComponent */
	void SetTargetSpline(const AActor* TargetActor);

	/** Starts platform movement.
	  * @warning it doesn't work if MovementMode is Manual.
	  */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void StartMovement();
	/** Stops platform movement.
	  * @warning the platform will stop movement only after arriving at the next point.
	  */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void StopMovement();
	/** Starts platform movent towards the given point.
	 *@warning works only in manual Movement Mode.
	 */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void MoveToPoint(const int32 TargetPointIndex);
	/** Checks if the given PointIndex isn't out of array bounds. */
	UFUNCTION(BlueprintPure, Category="FloatingPlatform")
	bool IsPointIndexOutOfBounds(const int32 PointIndex) const;
	/** Sets speed of a platform and recalculates TravelTime. */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void SetSpeed(int32 NewSpeed);

	/** Calls when a platform starts moving. */
	UPROPERTY(BlueprintAssignable, Category="Floating Platfrom")
	FOnPlatformStartMovement OnPlatformStartMovement;
	/** Calls when a platform stops moving. */
	UPROPERTY(BlueprintAssignable, Category="Floating Platform")
	FOnPlatformStopMovement OnPlatformStopMovement;
	/** Calls when a platform arrives at point. */
	UPROPERTY(BlueprintAssignable, Category="Floating Platform")
	FOnPlatformArrivedAtPoint OnPlatformArrivedAtPoint;
	/** Calls when a platform starts waiting at a point. */
	UPROPERTY(BlueprintAssignable, Category="Floating Platform")
	FOnPlatformWaitStarted OnPlatformWaitStarted;
	/** Calls when a platform finishes waiting at a point. */
	UPROPERTY(BlueprintAssignable, Category="Floating Platform")
	FOnPlatformWaitFinished OnPlatformWaitFinished;

protected:
	/** Calls when a platform starts moving. */
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnStartMovement();
	/** Calls when a platform stops moving. */
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnStopMovement();
	/** Calls when a platform starts waiting at a point. */
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnWaitStarted();
	/** Calls when a platform finishes waiting at a point. */
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnWaitFinished();
	/** Calls when a platform arrives at a point. */
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnArrivedAtPoint(const int32 PointIndex);

	/** Returns the last point of a TargetSpline. */
	int32 GetTargetSplineLastPoint() const;
	/** Returns the current position along TargetSpline.
	 * @param PathProgress — current movement progress between spline points.
	 */
	float GetCurrentSplinePosition(const float PathProgress) const;
	/** Sets actor location along TargetSpline.
	 *@param PathProgress — current movement progress between spline points.
	 */
	void SetLocationAlongSpline(const float PathProgress);
	/** Sets actor rotation along TargetSpline.
	 *@param PathProgress — current movement progress between spline points.
	 */
	void SetRotationAlongSpline(const float PathProgress);
	/** Moves and rotates a platform along TargetSpline.
	 *@param PathProgress — current movement progress between spline points.
	 */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void MoveAndRotateAlongSpline(const float PathProgress);
	/** Continues movement on the spline after arriving at the target point.
	 * Calculates point indexes and starts the wait timer if WaitDuration > 0.f.
	 */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void ContinueMovementAlongSpline();

private:
	/** An Actor which has a SplineComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	AActor* ActorWithSpline{this};
	/** SplineComponent along which a platform will move. */
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	USplineComponent* TargetSpline{nullptr};

	/** Current state of a platform. */
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	EFloatingPlatformState CurrentState{EFloatingPlatformState::Idle};

	/** Movement mode of a platform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	EFloatingPlatformMode MovementMode{EFloatingPlatformMode::Manual};
	/** Determines if start platform movement on BeginPlay. */ 
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", EditCondition="MovementMode != EFloatingPlatformMode::Manual")
	)
	bool bAutoStart{false};
	/** Determines if the movement is reversed or not.
	 * If true a platform will move from point 0 to the last point.
	 * Else a platform will move from the last point to point 0.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", EditCondition="MovementMode != EFloatingPlatformMode::Manual")
	)
	bool bIsReversed{false};

	/** Amount of time a platform will wait after arriving at point.
	 *It doesn't work in Manual MovementMode.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", ClampMin = "0.0", EditCondition="MovementMode != EFloatingPlatformMode::Manual"
		)
	)
	float WaitDuration{3.f};
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	FTimerHandle WaitTimerHandle{};
	/** Starts the wait timer */
	void StartWaitTimer();
	/** Processes the wait timer finish */
	void FinishWaitTimer();

	/** Index of a point from which a platform starts it's movement */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", ClampMin="0")
	)
	int32 StartPointIndex{0};
	/** Checks StartPointIndex if it has illegal value */
	void CheckStartPointIndex();
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	int32 PreviousPointIndex{0};
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	int32 NextPointIndex{0};
	/** Calculates next point index */
	void CalculateNextPointIndex(const int32 PointIndex);
	/** Calculates point indexes */
	void CalculatePointIndex();
	/** Set of points in which a platform will stop. */
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	TArray<int32> PathPoints{};
	/** Fills PathPoints with TargetSpline points' indexes. */
	void FillPathPoints();
	/** Determines if a platform will stops at custom points. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bUseCustomPathPoints{false};
	/** Set of custom points. Contains TargetSpline points' indexes */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", EditCondition="bUseCustomPathPoints")
	)
	TSet<int32> CustomPathPoints{};

	/** Movement speed of a platform */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", ClampMin="0.0")
	)
	float Speed{300.f};
	/** Travel time between points */
	UPROPERTY(BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	float TravelTime{1.f};
	/** Calculates travel time between two points */
	void CalculateTravelTime();

	/** Determines if a platform uses pitch rotation from TargetSpline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritPitch{false};
	/** Determines if a platform uses yaw rotation from TargetSpline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritYaw{false};
	/** Determines if a platform uses roll rotation from TargetSpline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritRoll{false};

	/** Process construction script.
	 *@warning Call it only in construction script.
	 */
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void ProcessConstructionScript();
	/** Prints error if TargetSpline == nullptr. */
	void PrintSplineNullError() const;
};
