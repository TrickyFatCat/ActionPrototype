// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Left,
	Right
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Damage")
	float Damage{5.f};
	UFUNCTION()
	void EnableCollision() const;
	UFUNCTION()
	void DisableCollision() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	EWeaponSlot WeaponSlot{EWeaponSlot::Left};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Damage", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UDamageType> DamageTypeClass{nullptr};
	
	UFUNCTION()
	void DealDamage(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* WeaponCollision{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* SkeletalMesh{nullptr};
};
