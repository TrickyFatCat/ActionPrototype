// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

class APawn;
/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	
	UFUNCTION(BlueprintPure, Category="Movement")
	FORCEINLINE float GetMovementSpeed() const { return MovementSpeed; }
	UFUNCTION(BlueprintPure, Category="Movement")
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }
	UFUNCTION(BlueprintPure, Category="Movement")
	FORCEINLINE APawn* GetPawnOwner() const { return PawnOwner; }

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Movement")
	void UpdateAnimationProperties();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	APawn* PawnOwner{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float MovementSpeed{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	bool bIsInAir{false};
};
