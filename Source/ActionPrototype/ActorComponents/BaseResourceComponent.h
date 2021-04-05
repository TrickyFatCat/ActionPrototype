// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValueIncreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValueDecreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxValueIncreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxValueDecreased, float, Amount, float, NewValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONPROTOTYPE_API UBaseResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseResourceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	float GetCurrentValue() const;
	float GetMaxValue() const;
	/** Increases CurrentValue on a given number.
	 *  @param Amount - delta value;
	 *  @param bClampToMax - determines if CurrentValue must be limited to MaxValue;
	 */
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void IncreaseValue(const float Amount, const bool bClampToMax = true);
	/** Decreases CurrentValue on a given number. */
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void DecreaseValue(const float Amount);
	/** Increases MaxValue on a given number.
	 *  @param Amount - delta value;
	 *  @param bClampCurrentValue — determines if CurrentValue must be changed with MaxValue;
	 */
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void IncreaseMaxValue(const float Amount, const bool bClampCurrentValue = true);
	/** Decreases MaxValue on a given number
	 * @param Amount - delta value;
	 * @param bClampCurrentValue - determines if CurrentValue must be changed with MaxValue;
	 */
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void DecreaseMaxValue(const float Amount, const bool bClampCurrentValue = true);
	/** Returns normalized value of a resource. */
	UFUNCTION(BlueprintPure, Category="Resource Component")
	float GetNormalizedValue() const;
	/** Return threshold value. */
	UFUNCTION(BlueprintPure, Category="Resource Component")
	float GetThresholdValue() const;
	/** Sets RestoreFrequency and calculates ChangeDelayTime. */
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	float SetRestoreFrequency(float NewRestoreFrequency);

	/** Calls when CurrentValue increased. */
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnValueIncreased OnCurrentValueIncreased;
	/** Calls when CurrentValue decreased. */
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnValueDecreased OnCurrentValueDecreased;
	/** Calls when MaxValue increased. */
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnMaxValueIncreased OnMaxValueIncreased;
	/** Calls when MaxValue decreased. */
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnMaxValueDecreased OnMaxValueDecreased;

protected:

private:
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="1.0")
	)
	float MaxValue{100.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	float CurrentValue{MaxValue};

	/** Determines if the resource should begin play with a custom current value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bCustomInitialValue{false};
	/** Custom CurrentValue on begin play. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin = "0.0", EditCondition="bCustomInitialValue")
	)
	float InitialValue{MaxValue};

	/** Determines if a resource should change itself automatically. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bAutoChange{false};
	/** Determines if a resource should decreasing while changing automatically. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bIsDecreasing{false};

	/** Determines a minimal relative value of CurrentValue for autochange.
	 * If CurrentValue reaches this relative value, auto change will stop.
	 * This value used only if bIsDecreasing == true.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutoChange && bIsDecreasing")
	)
	/** Determines a maximum relative value of CurrentValue for autochange.
	 * If CurrentValue reaches this relative value, auto change will stop.
	 * This value is used if bIsDecreasing == false.
	 */
	float ChangeMinThreshold{0.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutoChange && !bIsDecreasing")
	)
	float ChangeMaxThreshold{1.f};
	/** Determines how many resource will be restore every tick during autochange. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutoChange")
	)
	float ChangeAmount{1.f};
	/** Determines frequency of change ticks. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutoChange")
	)
	float ChangeFrequency{1.f};
	/** Time between ticks, calculates automatically from ChangeFrequency. */
	UPROPERTY(BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	float ChangeDelayTime{1.f};
	UPROPERTY(BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	FTimerHandle ChangeTimerHandle{};
	UFUNCTION()
	void StartAutoChange();
	UFUNCTION()
	void StopAutoChange();
	UFUNCTION()
	void ChangeCurrentValue();

	/** Determines time before starting changing CurrentValue automatically. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutoChange")
	)
	float ChangeStartDelay{1.f};
	UPROPERTY(BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	FTimerHandle ChangeStartDelayHandle{};
	UFUNCTION()
	void StartDelayTimer();
	UFUNCTION()
	void StopDelayTimer();

	UFUNCTION()
	bool IsCurrentValueOutOfBounds() const;
	UFUNCTION()
	void ProcessAutoChange();
};
