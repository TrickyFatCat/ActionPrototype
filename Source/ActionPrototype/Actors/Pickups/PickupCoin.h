// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickupItem.h"
#include "PickupCoin.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API APickupCoin : public ABasePickupItem
{
	GENERATED_BODY()

protected:
	virtual void ActivatePickupEffect(APlayerCharacter* PlayerCharacter) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Coin Pickup", meta=(AllowPrivateAccess="true"))
	int32 Value{1};
};
