// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class ACTIONPROTOTYPE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* WeaponCollision{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* SkeletalMesh{nullptr};
};
