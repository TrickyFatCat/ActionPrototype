// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupWeapon.h"
#include "ActionPrototype/Characters/PlayerCharacter.h"

void APickupWeapon::ActivatePickupEffect(APlayerCharacter* PlayerCharacter)
{
	if (PlayerCharacter == nullptr)
	{
		return;
	}

	PlayerCharacter->EquipWeapon(WeaponClass);
}


