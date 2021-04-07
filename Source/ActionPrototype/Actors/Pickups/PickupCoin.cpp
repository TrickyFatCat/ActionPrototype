// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupCoin.h"

#include "ActionPrototype/Characters/PlayerCharacter.h"

void APickupCoin::ActivatePickupEffect(APlayerCharacter* PlayerCharacter)
{
	if (PlayerCharacter == nullptr)
	{
		return;
	}

	PlayerCharacter->IncreaseCoins(Value);
}
