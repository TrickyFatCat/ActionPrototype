// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelTransitionTrigger.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API ALevelTransitionTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level Data")
	FName TargetLevelName{"Level Name"};

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void LoadLevel(AActor* OverlappedActor, AActor* OtherActor);
};
