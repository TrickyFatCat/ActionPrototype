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
	/* In active sate floor switch can be pressed */
	Active UMETA(DisplayName = "Active"),
	/* In Pressed state switch is pressed */
	Pressed UMETA(DisplayName = "Pressed"),
	/* In Locked state switch generate overlap events and work as intended, but can't be pressed */
	Locked UMETA(DisplayName = "Locked"),
	/* In this state the floor switch is transitioning from Active to Pressed */
	Transition UMETA(DisplayName = "Transition"),
	/* In Disabled state switch doesn't generate overlap events and ignore everything */
	Disabled UMETA(DisplayName = "Disabled")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchActive);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchLocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchUnlocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchDisabled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchEnabled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchTransitionStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchTransitionReverted);

UCLASS()
class ACTIONPROTOTYPE_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	AFloorSwitch();
	virtual void Tick(float DeltaTime) override;

	/* Called when a floor switch become pressed. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchPressed OnFloorSwitchPressed;
	/* Called when a floor switch enters Active state. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchActive OnFloorSwitchActive;
	/* Called when a floor switch become locked. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchLocked OnFloorSwitchLocked;
	/* Called when a floor switch become unlocked. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchUnlocked OnFloorSwitchUnlocked;
	/* Called when a floor switch become disabled. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchDisabled OnFloorSwitchDisabled;
	/* Called when a floor switch become enabled. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchEnabled OnFloorSwitchEnabled;
	/* Called when a floor switch starts its transition. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchTransitionStarted OnFloorSwitchTransitionStarted;
	/* Called when a floor switch reverts its transition. */
	UPROPERTY(BlueprintAssignable, Category="Floor Switch|Delegates")
	FOnSwitchTransitionReverted OnFloorSwitchTransitionReverted;

	/* Locks floor switch. In lock state it can't be pressed. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void LockFloorSwitch();
	/* Unlocks floor switch. It'll be transited in previous state before locking. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void UnlockFloorSwitch();
	/* Disables floor switch logic. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void DisableFloorSwitch();
	/* Enables floor switch logic. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void EnableFloorSwitch();

	/* Increases current number of presses. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	int32 IncreaseActivationNumber(const int32 Amount);
	/* Decreases current number of presses. */
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	int32 DecreaseActivationNumber(const int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	float SetTransitionTime(const float NewTime);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch")
	bool bActorIsInTrigger{false};

	// FUNCTIONS
	UFUNCTION(BlueprintNativeEvent)
	void TriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void TriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchActivated();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchPressed();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchLocked();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchUnlocked();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchDisabled();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchEnabled();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchTransitionStarted();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnSwitchTransitionReverted();
	UFUNCTION(BlueprintImplementableEvent, Category="Floor Switch")
	void OnTransitionTimeChanged();

	UFUNCTION(BlueprintPure, Category="Floor Switch")
	float CalculatePlayRate() const;

	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void UpdateButtonLocation(float OffsetX = 0.f, float OffsetY = 0.f, float OffsetZ = 0.f);


private:
	// COMPONENTS
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolume{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SwitchMesh{nullptr};

	// PROPERTIES
	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	FVector InitialMeshLocation{FVector::ZeroVector};
	
	/* Determines time of transition between Active and Pressed states. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor Switch",
		meta=(AllowPrivateAccess = "true", ClampMin = "0.0"))
	float TransitionTime{5.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	bool bCanTransitionBeReverted{false};
	
	/* If true, switch can be pressed limited number of times until being disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bIsActivationLimited{false};
	/* Determines how many times switch can be pressed on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch",
		meta=(AllowPrivateAccess = "true", ClampMin = "1", EditCondition="bIsActivationLimited"))
	int32 InitialActivationNumber{1};
	/* Remaining number of presses. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch",
		meta=(AllowPrivateAccess = "true", EditCondition="bIsActivationLimited"))
	int32 ActivationNumber{InitialActivationNumber};

	/* How long switch will stay in Pressed state after player left the switch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", EditCondition="bIsPressedTemporary"))
	float PressedDuration{5.f};

	/* Initial state of the switch on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch|States", meta = (AllowPrivateAccess = "true"))
	EFloorSwitchState InitialSwitchState{EFloorSwitchState::Active};
	/* Current switch state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch|States", meta = (AllowPrivateAccess = "true"))
	EFloorSwitchState CurrentSwitchState{EFloorSwitchState::Active};
	/* Previous switch sate before locking. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch|States", meta = (AllowPrivateAccess = "true"))
	EFloorSwitchState PreviousSwitchState;
	/* Target state for transition. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch|States", meta=(AllowPrivateAccess="true"))
	EFloorSwitchState TargetSwitchState;

	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess = "true"))
	FTimerHandle PressedTimerHandle;
	FTimerDelegate PressedTimerDelegate;

	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess="true"))
	FTimerHandle TransitionTimerHandle;
	FTimerDelegate TransitionTimerDelegate;

	// FUNCTIONS
	UFUNCTION()
	void ChangeStateTo(const EFloorSwitchState NewState);
	UFUNCTION()
	void StartTransition();
	UFUNCTION()
	void RevertTransition();
};
