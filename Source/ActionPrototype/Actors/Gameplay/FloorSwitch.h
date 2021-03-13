// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FunctionalTestingManager.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ESwitchState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	// In this state switch is ready to be pressed
	Pressed UMETA(DisplayName = "Pressed"),
	// In this state switch is already pressed
	Disabled UMETA(DisplayName = "Disabled"),
	// This state means that the switch is completely disabled and don't work at all
	Locked UMETA(DisplayName = "Locked"),
	// In this state switch can't be pressed, but it can be unlocked
	Transition UMETA(DisplayName = "Transition")
	// This sate means that the switch is transitioning between states, it can't be locked, unlocked or disabled
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchPressed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchPressing);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchLocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchUnlocked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchDisabled);

UCLASS()
class ACTIONPROTOTYPE_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	AFloorSwitch();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Floor Switch | Delegates")
	FOnSwitchPressed OnSwitchPressed;
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Floor Switch | Delegates")
	FOnSwitchPressing OnSwitchPressing;
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Floor Switch | Delegates")
	FOnSwitchLocked OnSwitchLocked;
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Floor Switch | Delegates")
	FOnSwitchUnlocked OnSwitchUnlocked;
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Floor Switch | Delegates")
	FOnSwitchDisabled OnSwitchDisabled;

	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void LockFloorSwitch();
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void UnlockFloorSwitch();
	UFUNCTION(BlueprintCallable, Category="Floor Switch")
	void DisableFloorSwitch();

protected:
	virtual void BeginPlay() override;

	// FUNCTIONS
	UFUNCTION(BlueprintNativeEvent)
	void TriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void TriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolume{nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SwitchMesh{nullptr};

	// PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bTriggerOnce{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bRequirePressing{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bIsTransitionInterruptible{false};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch", meta = (AllowPrivateAccess = "true"))
	ESwitchState InitialSwitchState = ESwitchState::Normal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor Switch", meta = (AllowPrivateAccess = "true"))
	ESwitchState CurrentSwitchState = ESwitchState::Normal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	ESwitchState PreviousSwitchState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch", meta = (AllowPrivateAccess = "true"))
	bool bIsPressedTemporary{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Floor Switch",
		meta = (AllowPrivateAccess = "true", ClampMin = "1.0", EditCondition="bIsPressedTemporary"))
	float PressedDuration{5.f};

	UPROPERTY(BlueprintReadOnly, Category="Floor Switch", meta=(AllowPrivateAccess = "true"))
	FTimerHandle PressedDurationTimer;

	// FUNCTIONS
	UFUNCTION()
	void ChangeStateToNormal();
};
