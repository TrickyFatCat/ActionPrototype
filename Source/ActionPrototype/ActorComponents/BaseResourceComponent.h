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
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void SetMaxValue(const float NewMaxValue);
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void IncreaseValue(const float Amount, const bool bClampToMax = true);
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void DecreaseValue(const float Amount);
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void IncreaseMaxValue(const float Amount, const bool bClampCurrentValue = true);
	UFUNCTION(BlueprintCallable, Category="Resource Component")
	void DecreaseMaxValue(const float Amount, const bool bClampCurrentValue = true);
	UFUNCTION(BlueprintPure, Category="Resource Component")
	float GetNormalizedValue() const;
	UFUNCTION(BlueprintPure, Category="Resource Component")
	float GetThresholdValue() const;

	UFUNCTION(BlueprintCallable, Category="Resource Component")
	float SetRestoreFrequency(float NewRestoreFrequency);

	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnValueIncreased OnCurrentValueIncreased;
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnValueDecreased OnCurrentValueDecreased;
	UPROPERTY(BlueprintAssignable, Category="Resource Component|Delegates")
	FOnMaxValueIncreased OnMaxValueIncreased;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bCustomInitialValue{false};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin = "0.0", EditCondition="bCustomInitialValue")
	)
	float InitialValue{MaxValue};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bAutoChange{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	bool bIsDecreasing{false};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutoChange && bIsDecreasing")
	)
	float ChangeMinThreshold{0.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutoChange && !bIsDecreasing")
	)
	float ChangeMaxThreshold{MaxValue};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutoChange")
	)
	float ChangeAmount{1.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutoChange")
	)
	float ChangeFrequency{1.f};
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
