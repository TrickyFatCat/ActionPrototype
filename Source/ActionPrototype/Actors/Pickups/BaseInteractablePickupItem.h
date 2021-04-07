// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickupItem.h"
#include "BaseInteractablePickupItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API ABaseInteractablePickupItem : public ABasePickupItem, public IReactToInteraction
{
	GENERATED_BODY()
protected:
	void ProcessInteraction_Implementation(APlayerCharacter* PlayerCharacter) override;
};
