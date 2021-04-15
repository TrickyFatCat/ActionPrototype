// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupPotion.h"

#include "ActionPrototype/Characters/PlayerCharacter.h"

void APotionPickup::ActivatePickupEffect(APlayerCharacter* PlayerCharacter)
{
	if (RestoreHealth > 0.f)
	{
		PlayerCharacter->IncreaseCurrentHealth(RestoreHealth, bRestoreOverMax);
	}

	if (RestoreStamina > 0.f)
	{
		PlayerCharacter->IncreaseStamina(RestoreStamina);
	}
}
