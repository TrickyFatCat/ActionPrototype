// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractablePickupItem.h"

void ABaseInteractablePickupItem::ProcessInteraction_Implementation(APlayerCharacter* PlayerCharacter)
{
	ProcessPickup(PlayerCharacter);
}
