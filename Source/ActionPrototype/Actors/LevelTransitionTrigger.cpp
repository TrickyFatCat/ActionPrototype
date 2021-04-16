// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionTrigger.h"

#include "ActionPrototype/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void ALevelTransitionTrigger::BeginPlay()
{
    OnActorBeginOverlap.AddDynamic(this, &ALevelTransitionTrigger::LoadLevel);
}

void ALevelTransitionTrigger::LoadLevel(AActor* OverlappedActor, AActor* OtherActor)
{
    const UWorld* World = GetWorld();
    
    if (OtherActor == nullptr || World == nullptr)
    {
        return;
    }

    const FName CurrentLevelName = FName(*World->GetMapName());

    if (CurrentLevelName != TargetLevelName)
    {
        UGameplayStatics::OpenLevel(World, TargetLevelName);
    }
}

