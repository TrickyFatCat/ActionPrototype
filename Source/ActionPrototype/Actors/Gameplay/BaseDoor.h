// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "BaseDoor.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Opened UMETA(DisplayName="Opened"),
	Closed UMETA(DisplayName="Closed"),
	Locked UMETA(DisplayName="Locked"),
	Transition UMETA(DisplayName="Transition"),
	Disabled UMETA(DisplayName="Disabled")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpened);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorLocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorTransitionStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorTransitionReverted);

UCLASS()
class ACTIONPROTOTYPE_API ABaseDoor : public AActor
{
	GENERATED_BODY()

public:
	ABaseDoor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Door")
	bool OpenDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	bool CloseDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	bool LockDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	bool UnlockDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	bool DisableDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	bool EnableDoor(const EDoorState NewState);
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetTransitionDuration(const float NewDuration);

	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorClosed OnDoorClosed;
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorOpened OnDoorOpened;
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorLocked OnDoorLocked;
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorTransitionStarted OnDoorTransitionStarted;
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorTransitionReverted OnDoorTransitionReverted;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnOpened();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnClosed();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnLocked();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnUnlocked();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnDisabled();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnEnabled();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionStarted();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionReverted();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionDurationChanged();
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnStateChanged();

	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorLocation(UStaticMeshComponent* DoorMesh, const FVector InitialLocation, const FVector LocationOffset);
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorRotation(UStaticMeshComponent* DoorMesh, const FRotator InitialRotation, const FRotator RotationOffset);
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorLocationAndRotation(
			UStaticMeshComponent* DoorMesh,
			const FVector InitialLocation,
			const FVector LocationOffset,
			const FRotator InitialRotation,
			const FRotator RotationOffset
		);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState InitialState{EDoorState::Closed};
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState CurrentState{InitialState};
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState TargetState;
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState PreviousState;
	UFUNCTION()
	void ChangeStateTo(const EDoorState NewState);
	void SetTargetState(const EDoorState State);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess="true"))
	float TransitionDuration{0.25f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true"))
	bool bIsTransitionRevertible{false};
	UFUNCTION()
	void StartTransition();
	UFUNCTION()
	void RevertTransition();
	UFUNCTION(BlueprintCallable, Category="Door")
	void FinishTransition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true", ClampMin = "0.0"))
	float CloseDelay{5.f};
	UPROPERTY(BlueprintReadOnly, Category="Door|Time Handles", meta=(AllowPrivateAccess="true"))
	FTimerHandle CloseDelayHandle;
};
