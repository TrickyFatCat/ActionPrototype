// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseInteractablePickupItem.h"
#include "BasePickupItem.h"
#include "ActionPrototype/Actors/Weapon.h"

#include "PickupWeapon.generated.h"

UCLASS()
class ACTIONPROTOTYPE_API APickupWeapon : public ABaseInteractablePickupItem 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon pickup")
	EWeaponSlot WeaponSlot{EWeaponSlot::Right};
protected:
	virtual void ActivatePickupEffect(APlayerCharacter* PlayerCharacter) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon pickup", meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> WeaponClass{nullptr};
};
