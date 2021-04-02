// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseResourceComponent.generated.h"


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

	UFUNCTION(BlueprintCallable, Category="Resource Component")
	float SetRestoreFrequency(float NewRestoreFrequency);

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
	bool bAutorestore{false};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutorestore")
	)
	float RestoreMinThreshold{0.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", ClampMax="1", EditCondition="bAutorestore")
	)
	float RestoreMaxThreshold{MaxValue};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutorestore")
	)
	float RestoreAmount{1.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category="Resource Component",
		meta=(AllowPrivateAccess="true", ClampMin="0", EditCondition="bAutorestore")
	)
	float RestoreFrequency{1.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	float RestoreDelayTime{1.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	FTimerHandle RestoreTimerHandle{};
	UFUNCTION()
	void StartAutoRestore();
	UFUNCTION()
	void StopAutoRestore();
	UFUNCTION()
	void RestoreResource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	float RestoreStartDelay{1.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource Component", meta=(AllowPrivateAccess="true"))
	FTimerHandle RestoreStartDelayHandle{};
	UFUNCTION()
	void StartDelayTimer();
	UFUNCTION()
	void StopDelayTimer();
};
