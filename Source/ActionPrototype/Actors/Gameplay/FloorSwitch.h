// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FunctionalTestingManager.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EFloorSwitchState : uint8
{
	/* In this state a switch is pressed */
	Pressed UMETA(DisplayName = "Pressed"),
	/* In this state switch is ready to be press */
	Idle UMETA(DisplayName = "Idle"),
	/* In this state a switch generates overlap events and works as intended, but can't be pressed */
	Locked UMETA(DisplayName = "Locked"),
	/* In this state a switch is transitioning from Active to Pressed */
	Transition UMETA(DisplayName = "Transition"),
	/* In this state a switch doesn't generate overlap events and ignore everything */
	Disabled UMETA(DisplayName = "Disabled")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchIdle);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchLocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchTransitionStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchTransitionReverted);

UCLASS()
class ACTIONPROTOTYPE_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	AFloorSwitch();
	virtual void Tick(float DeltaTime) override;

	/* Called when a switch changes its state to Idle */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchIdle OnFloorSwitchIdle;
	/* Called when a switch changes its state to Pressed */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchPressed OnFloorSwitchPressed;
	/* Called when a switch changes its state to Locked */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchLocked OnFloorSwitchLocked;
	/* Called when a switch changes its state to Transition */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchTransitionStarted OnFloorSwitchTransitionStarted;
	/* Called when the transition reverted */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchTransitionReverted OnFloorSwitchTransitionReverted;

	/* Locks floor switch. In lock state it can't be pressed. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void LockFloorSwitch();
	/* Unlocks floor switch. It'll be transited in previous state before locking. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void UnlockFloorSwitch(const EFloorSwitchState NewState);
	/* Disables floor switch logic. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void DisableFloorSwitch();
	/* Enables floor switch logic. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void EnableFloorSwitch();

	/* Increases current number of presses. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	int32 IncreasePressesNumber(const int32 Amount);
	/* Decreases current number of presses. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	int32 DecreasePressesNumber(const int32 Amount);

	/* Sets transition time */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	float SetTransitionTime(const float NewTime);

protected:
	virtual void BeginPlay() override;

	// FUNCTIONS
	UFUNCTION()
	void TriggerOverlapBegin(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);
	UFUNCTION()
	void TriggerOverlapEnd(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

	/* Called when a switch changes state to Active */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnIdle();
	/* Called when a switch changes state to Pressed */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnPressed();
	/* Called when a switch changes state to Locked */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnLocked();
	/* Called in UnlockFloorSwitch function */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnUnlocked();
	/* Called when a switch changes state to Disabled*/
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnDisabled();
	/* Called in EnableFloorSwitch function */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnEnabled();
	/* Called when a switch changes state to Transition*/
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnTransitionStarted();
	/* Called when the transition reverted */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnTransitionReverted();
	/* Called in SetTransitionTime function */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnTransitionTimeChanged();
	/* Called when a switch changes its state */
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnStateChanged();

	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void UpdateButtonLocation(const float OffsetX = 0.f, const float OffsetY = 0.f, const float OffsetZ = 0.f) const;

private:
	// COMPONENTS
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolume{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SwitchMesh{nullptr};

	// PROPERTIES
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	FVector InitialMeshLocation{FVector::ZeroVector};
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	bool bActorIsInTrigger{false};

	/* Initial state of the switch on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	EFloorSwitchState InitialSwitchState{EFloorSwitchState::Idle};
	/* Current switch state. */
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch|States", meta=(AllowPrivateAccess = "true"))
	EFloorSwitchState CurrentSwitchState{EFloorSwitchState::Idle};
	/* Previous switch sate before locking. */
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch|States", meta=(AllowPrivateAccess = "true"))
	EFloorSwitchState PreviousSwitchState;
	/* Target state for transition. */
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch|States", meta=(AllowPrivateAccess="true"))
	EFloorSwitchState TargetSwitchState;

	/* Determines time of transition between Active and Pressed states. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Floor Switch",
		meta=(AllowPrivateAccess = "true", ClampMin = "0.0")
	)
	float TransitionTime{5.f};
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	FTimerHandle TransitionTimerHandle;
	FTimerDelegate TransitionTimerDelegate;
	/* Determines if Transition can be reverted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	bool bIsTransitionRevertible{false};

	/* Determines the duration of changing the state from Active to Transition when an Actor steps into the trigger.
	   If == 0.0 state changes immediately.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta=(AllowPrivateAccess = "true"))
	float PressDelay{0.5f};
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	FTimerHandle PressDelayHandle;
	void ClearPressDelayHandle();

	/* How long switch will stay in Pressed state after the player left the trigger.
       If == 0.0 state changes immediately.
    */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floor Switch",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0")
	)
	float PressedDuration{5.f};
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess = "true"))
	FTimerHandle PressedDurationHandle;
	void SetPressedTimer();

	/* If true, switch can be pressed limited number of times until being disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bLimitedPresses{false};
	/* Determines how many times switch can be pressed */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Floor Switch",
		meta=(AllowPrivateAccess = "true", ClampMin = "1", EditCondition="bLimitedPresses")
	)
	int32 InitialPressesNumber{1};
	/* Remaining number of presses. */
	UPROPERTY(
		BlueprintReadOnly,
		Category="Floor Switch",
		meta=(AllowPrivateAccess = "true")
	)
	int32 PressesNumber{InitialPressesNumber};

	// FUNCTIONS
	UFUNCTION()
	void ChangeStateTo(const EFloorSwitchState NewState);
	UFUNCTION()
	void StartTransition();
	UFUNCTION()
	void RevertTransition();
};
