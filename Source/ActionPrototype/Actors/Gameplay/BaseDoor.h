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

	/* Starts the door transition to the Opened state */
	UFUNCTION(BlueprintCallable, Category="Door")
	bool OpenDoor();
	/* Starts the door transition to the Closed state */
	UFUNCTION(BlueprintCallable, Category="Door")
	bool CloseDoor();
	/* Transits a door to the Locked state. It works only if a door is in the Closed state */
	UFUNCTION(BlueprintCallable, Category="Door")
	bool LockDoor();
	/* Transits a door from the Locked state to the Closed state */
	UFUNCTION(BlueprintCallable, Category="Door")
	bool UnlockDoor();
	/* Transits a door to the Disabled state. It works only if a door is in the Closed or the Opened state */
	UFUNCTION(BlueprintCallable, Category="Door")
	bool DisableDoor();
	UFUNCTION(BlueprintCallable, Category="Door")
	/* Transits a door from the Disabled state to a given state. */
	bool EnableDoor(const EDoorState NewState);
	/* Sets transition time of the door */
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetTransitionDuration(const float NewDuration);

	/* Calls when a door enters the Closed state */
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorClosed OnDoorClosed;
	/* Calls when a door enters the Opened state */
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorOpened OnDoorOpened;
	/* Calls when a door enters the Locked state */
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorLocked OnDoorLocked;
	/* Calls when a door enters the Transition state */
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorTransitionStarted OnDoorTransitionStarted;
	/* Calls when TargetState was changed during the Transition state */
	UPROPERTY(BlueprintAssignable, Category="Door|Delegates")
	FOnDoorTransitionReverted OnDoorTransitionReverted;

protected:
	virtual void BeginPlay() override;

	/* Calls when a door enters the Opened state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnOpened();
	/* Calls when a door enters the Closed state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnClosed();
	/* Calls when a door enters the Locked state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnLocked();
	/* Calls when a door exits the Locked state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnUnlocked();
	/* Calls when a door enters the Disabled state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnDisabled();
	/* Calls when a door exits the Disabled state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnEnabled();
	/* Calls when a door enters the Transition state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionStarted();
	/* Calls every Timeline tick in a Transition state
	 * Must be implemented in Blueprint class */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Door")
	void OnTransitionUpdate(float TransitionProgress);
	/* Calls when TargetState was changed during the Transition state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionReverted();
	/* Calls when a door exits the Transition state */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionFinished();
	/* Calls when TransitionDuration changed */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTransitionDurationChanged();
	/* Calls when a door changes its CurrentState */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnStateChanged();

	/* Sets world location of the given static mesh
	 * @param DoorMesh — door's leaf static mesh.
	 * @param InitialLocation — initial location of DoorMesh on BeginPlay.
	 * @param LocationOffset — the distance on which DoorMesh is moved. */
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorLocation(UStaticMeshComponent* DoorMesh, const FVector InitialLocation, const FVector LocationOffset);
	/* Sets world rotation of the given static mesh
	 * @param DoorMesh — door's leaf static mesh.
	 * @param InitialRotation — initial rotation of DoorMesh on BeginPlay.
	 * @param LocationOffset — the angle on which DoorMesh is rotated. */
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorRotation(UStaticMeshComponent* DoorMesh, const FRotator InitialRotation, const FRotator RotationOffset);
	/* Sets world location and rotation of the given static mesh
	 * @param DoorMesh — door's leaf static mesh.
	 * @param InitialLocation — initial location of DoorMesh on BeginPlay.
	 * @param LocationOffset — the distance on which DoorMesh is moved.
	 * @param InitialRotation — initial rotation of DoorMesh on BeginPlay.
	 * @param LocationOffset — the angle on which DoorMesh is rotated. */
	UFUNCTION(BlueprintCallable, Category="Door")
	void SetDoorLocationAndRotation(
			UStaticMeshComponent* DoorMesh,
			const FVector InitialLocation,
			const FVector LocationOffset,
			const FRotator InitialRotation,
			const FRotator RotationOffset
		);

private:
	/* Initial state of a door in which it enters on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState InitialState{EDoorState::Closed};
	/* Current state of a door */
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState CurrentState{InitialState};
	/* A state in which a door enters when a door finishes transition process */
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState TargetState;
	/* A state in witch a door was before entering current state */
	UPROPERTY(BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess = "true"))
	EDoorState PreviousState;
	/* Changes CurrentState to a given state */
	UFUNCTION()
	void ChangeStateTo(const EDoorState NewState);
	void SetTargetState(const EDoorState State);

	/* Determines a door's transition duration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess="true"))
	float TransitionDuration{0.25f};
	/* Determines if TargetState can be changed during the Transition state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true"))
	bool bIsTransitionRevertible{false};
	/* Changes CurrentState to the Transition state */
	UFUNCTION()
	void StartTransition();
	/* Changes TargetState while the Transition state */
	UFUNCTION()
	void RevertTransition();
	/* Changes CurrentState to TargetState value.
	 * @warning It must be called in Blueprints in order to finish transition. */
	UFUNCTION(BlueprintCallable, Category="Door")
	void FinishTransition();

	/* Determines how long a door stays in the Opened state before calling CloseDoor() automatically.
	 * @warning If it's equal 0 CloseDoor() won't be called automatically. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door", meta=(AllowPrivateAccess = "true", ClampMin = "0.0"))
	float CloseDelay{5.f};
	/* CloseDelay timer handle */
	UPROPERTY(BlueprintReadOnly, Category="Door|Time Handles", meta=(AllowPrivateAccess="true"))
	FTimerHandle CloseDelayHandle;
};
