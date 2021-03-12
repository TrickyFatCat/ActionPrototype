// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (PawnOwner == nullptr)
	{
		PawnOwner = TryGetPawnOwner();
	}
}

void UBaseAnimInstance::UpdateAnimationProperties_Implementation()
{
	if (PawnOwner == nullptr)
	{
		PawnOwner = TryGetPawnOwner();
	}

	if (PawnOwner != nullptr)
	{
		const FVector PawnVelocity = PawnOwner->GetVelocity();
		const FVector PawnLateralVelocity = FVector(PawnVelocity.X, PawnVelocity.Y, 0.f);
		MovementSpeed = PawnLateralVelocity.Size();

		bIsInAir = PawnOwner->GetMovementComponent()->IsFalling();
	}
}
