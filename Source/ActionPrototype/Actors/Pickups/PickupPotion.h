// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickupItem.h"
#include "PickupPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API APotionPickup : public ABasePickupItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion Parameters")
	float RestoreHealth{25.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion Parameters")
	bool bRestoreOverMax{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion Parameters")
	float RestoreStamina{25.f};
protected:
	virtual void ActivatePickupEffect(APlayerCharacter* PlayerCharacter) override;
};
