// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionPlayerController.h"

#include "Blueprint/UserWidget.h"

void AActionPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDClass != nullptr)
	{
		PlayerHUD = CreateWidget<UUserWidget>(this, PlayerHUDClass);
		PlayerHUD->AddToViewport();
		PlayerHUD->SetVisibility(ESlateVisibility::Visible);
	}
}


