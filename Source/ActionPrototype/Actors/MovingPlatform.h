// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

class USplineComponent;

UENUM(Blueprintable)
enum class EPlatformMode : uint8
{
	OneWay,
	Loop,
	ReverseLoop,
	Manual
};

UCLASS()
class ACTIONPROTOTYPE_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnStartMovement();
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnArrivedInPoint(const int32 PointIndex);
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnWaitStart();
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnWaitFinish();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USplineComponent* PlatformPath{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PlatformMesh{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	EPlatformMode InitialMode{EPlatformMode::OneWay};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	EPlatformMode CurrentMode{EPlatformMode::OneWay};
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void SetMovingPlatformMode(const EPlatformMode NewMode);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	bool bIsReversed{false};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Moving Platform",
		meta=(AllowPrivateAccess = "true", ClampMin = "0")
	)
	int32 StartPoint{0};
	UPROPERTY(BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	int32 PreviousPoint{0};
	UPROPERTY(BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	int32 TargetPoint{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	TSet<int32> StopoverPointsSet{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	TArray<int32> StopoverPointsArray{};
	int32 GetLastIndex() const;
	int32 GetLastPoint() const;
	bool IsTargetPointOutOfBounds() const;
	void ContinueMovement();
	void CalculateNextPoint();
	void CalculateNextStopover();
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void ChangeTargetPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	bool bInheritPitch{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	bool bInheritYaw{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	bool bInheritRoll{false};
	float GetCurrentPlatformPosition(const float PathProgress) const;
	void MoveAlongSpline(const float PathProgress) const;
	void RotateAlongSpline(const float PathProgress) const;
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void MovePlatform(const float PathProgress);

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Moving Platform",
		meta=(AllowPrivateAccess = "true", ClampMin = "0.0")
	)
	float WaitDuration{0.f};
	UPROPERTY(BlueprintReadOnly, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	FTimerHandle WaitDurationHandle{};
	void StartWaitTimer();
};
