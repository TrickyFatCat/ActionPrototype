// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ActionPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API AActionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="User Interface")
	TSubclassOf<UUserWidget> PlayerHUDClass{nullptr};

	UPROPERTY(BlueprintReadOnly, Category="User Interface")
	UUserWidget* PlayerHUD{nullptr};

protected:
	virtual void BeginPlay() override;
	
};
