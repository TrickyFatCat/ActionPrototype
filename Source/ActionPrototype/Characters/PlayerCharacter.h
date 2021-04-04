// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBaseResourceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaIncreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaDecreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoinsIncreased, int32, Amount, int32, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoinsDecreased, int32, Amount, int32, NewValue);

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category="Player|Camera")
	FORCEINLINE float GetCameraYawSensitivity() const { return CameraYawSensitivity; }

	UFUNCTION(BlueprintPure, Category="Player|Camera")
	FORCEINLINE float GetCameraPitchSensitivity() const { return CameraPitchSensitivity; }

	UFUNCTION(BlueprintCallable, Category="Player|Camera")
	bool SetCameraYawSensitivity(const float NewSensitivity);
	UFUNCTION(BlueprintCallable, Category="Player|Camera")
	bool SetCameraPitchSensitivity(const float NewSensitivity);

	UFUNCTION(BlueprintPure, Category="Player|Stamina")
	float GetCurrentStamina() const;
	UFUNCTION(BlueprintPure, Category="Player|Stamina")
	float GetMaxStamina() const;
	UFUNCTION(BlueprintPure, Category="Player|Stamina")
	float GetNormalisedStamina() const;
	UFUNCTION(BlueprintCallable, Category="Player|Stamina")
	void DecreaseStamina(const float Amount);
	UFUNCTION(BlueprintCallable, Category="Player|Stamina")
	void IncreaseStamina(const float Amount);
	UFUNCTION(BlueprintCallable, Category="Player|Stamina")
	void IncreaseMaxStamina(const float Amount);
	UFUNCTION(BlueprintCallable, Category="Player|Stamina")
	void DecreaseMaxStamina(const float Amount);

	UFUNCTION(BlueprintCallable, Category="Player|Coins")
	void IncreaseCoins(const int32 Amount);
	UFUNCTION(BlueprintCallable, Category="Player|Coins")
	void DecreaseCoins(const int32 Amount);
	UFUNCTION(BlueprintPure, Category="Player|Coins")
	int32 GetCoins() const;

	UPROPERTY(BlueprintAssignable, Category="Player|Stamina")
	FOnStaminaIncreased OnStaminaIncreased;
	UPROPERTY(BlueprintAssignable, Category="Player|Stamina")
	FOnStaminaDecreased OnStaminaDecreased;
	UPROPERTY(BlueprintAssignable, Category="Player|Coins")
	FOnCoinsIncreased OnCoinsIncreased;
	UPROPERTY(BlueprintAssignable, Category="Player|Coins")
	FOnCoinsDecreased OnCoinsDecreased;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UBaseResourceComponent* StaminaComponent{nullptr};

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Player|Camera",
		meta=(AllowPrivateAccess = "true", ClampMin = "1.0")
	)
	float CameraYawSensitivity{50.f};
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="Player|Camera",
		meta=(AllowPrivateAccess = "true", ClampMin = "1.0")
	)
	float CameraPitchSensitivity{50.f};

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);
	void LookUp(float AxisValue);

	UFUNCTION()
	void BroadcastStaminaIncreased(const float Amount, const float NewValue);
	UFUNCTION()
	void BroadcastStaminaDecreased(const float Amount, const float NewValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Coins", meta=(AllowPrivateAccess="true"))
	int32 Coins{0};
};
