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
	ReverseLoop
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
	void OnPointPassed(const int32 PassedPoint);
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnPlatformWaitStart();
	UFUNCTION(BlueprintImplementableEvent, Category="Moving Platform")
	void OnPlatformWaitFinish();
	
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void ClearPassedPoints();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	TArray<float> PointsPositions{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Moving Platform", meta=(AllowPrivateAccess="true"))
	TArray<int32> PassedPoints{};
	void FillPointsPositions();
	UPROPERTY(BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	bool bIsReversed{false};
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void CheckPointsOnPath(const float PathProgress);
	void AddPointToPassedPoints(const int32 PointIndex);

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
	void ProcessPlatformMovement(const float PathProgress);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	float WaitDuration{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moving Platform", meta=(AllowPrivateAccess = "true"))
	FTimerHandle WaitDurationHandle{};
	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	void StartWaitTimer();
	
	
};
