// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseInteractablePickupItem.h"
#include "BasePickupItem.h"

#include "PickupWeapon.generated.h"

class AWeapon;
/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API APickupWeapon : public ABaseInteractablePickupItem 
{
	GENERATED_BODY()

protected:
	virtual void ActivatePickupEffect(APlayerCharacter* PlayerCharacter) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon pickup", meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> WeaponClass{nullptr};
	
};
