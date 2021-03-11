// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS(Blueprintable, Placeable)
class ACTIONPROTOTYPE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category="Player Camera")
	FORCEINLINE float GetCameraYawSensitivity() const { return CameraYawSensitivity; }
	UFUNCTION(BlueprintPure, Category="Player Camera")
	FORCEINLINE float GetCameraPitchSensitivity() const { return CameraPitchSensitivity; }

	UFUNCTION(BlueprintCallable, Category="Player Camera")
	bool SetCameraYawSensitivity(const float NewSensitivity);
	UFUNCTION(BlueprintCallable, Category="Player Camera")
	bool SetCameraPitchSensitivity(const float NewSensitivity);
protected:
	virtual void BeginPlay() override;

private:
	const float DefaultArmLength = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent{nullptr};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Sensitivity", meta=(AllowPrivateAccess = "true", ClampMin = "1.0"))
	float CameraYawSensitivity{50.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Sensitivyty", meta=(AllowPrivateAccess = "true", ClampMin = "1.0"))
	float CameraPitchSensitivity{50.f};
	
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);
	void LookUp(float AxisValue);
};
