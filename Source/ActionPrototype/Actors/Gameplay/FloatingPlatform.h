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

UENUM()
enum class EFloatingPlatformState : uint8
{
	Idle,
	Move,
	Wait
};

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
	void SetTargetSpline(const AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void StartMovement();
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void StopMovement();
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void MoveToPoint(const int32 TargetPointIndex);
	UFUNCTION(BlueprintPure, Category="FloatingPlatform")
	bool IsPointIndexOutOfBounds(const int32 PointIndex) const;
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void SetSpeed(int32 NewSpeed);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnStartMovement();
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnStopMovement();
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnWaitStarted();
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnWaitFinished();
	UFUNCTION(BlueprintImplementableEvent, Category="Floating Platform")
	void OnArriveAtPoint(const int32 PointIndex);

	int32 GetTargetSplineLastPoint() const;
	float GetCurrentSplinePosition(const float PathProgress) const;
	void SetLocationAlongSpline(const float PathProgress);
	void SetRotationAlongSpline(const float PathProgress);
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void MoveAndRotateAlongSpline(const float PathProgress);
	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void ContinueMovementAlongSpline();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	AActor* ActorWithSpline{this};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	USplineComponent* TargetSpline{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	EFloatingPlatformState CurrentState{EFloatingPlatformState::Idle};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	EFloatingPlatformMode MovementMode{EFloatingPlatformMode::Manual};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", EditCondition="MovementMode != EFloatingPlatformMode::Manual")
	)
	bool bIsReversed{false};

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
	void StartWaitTimer();
	void FinishWaitTimer();

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", ClampMin="0")
	)
	int32 StartPointIndex{0};
	void CheckStartPointIndex();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	int32 PreviousPointIndex{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	int32 NextPointIndex{0};
	void CalculateNextPointIndex(const int32 PointIndex);
	void CalculatePointIndex();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	TArray<int32> PathPoints{};
	void FillPathPoints();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bUseCustomPathPoints{false};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", EditCondition="bUseCustomPathPoints")
	)
	TSet<int32> CustomPathPoints{};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Floating Platform",
		meta=(AllowPrivateAccess="true", ClampMin="0.0")
	)
	float Speed{300.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	float TravelTime{1.f};
	void CalculateTravelTime();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritPitch{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritYaw{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floating Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritRoll{false};

	UFUNCTION(BlueprintCallable, Category="Floating Platform")
	void ProcessConstructionScript();
	void PrintSplineNullError() const;
};
