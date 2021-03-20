// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;
class AEnemyCharacter;

UCLASS()
class ACTIONPROTOTYPE_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintImplementableEvent, Category="Spawn Volume")
	AEnemyCharacter* OnEnemySpawned(AEnemyCharacter* SpawnedEnemy);
	
protected:
	UFUNCTION(BlueprintPure, Category="Spawn Volume")
	FVector GetRandomPoint() const;
	UFUNCTION(BlueprintCallable, Category="Spawn Volume")
	AEnemyCharacter* ProcessEnemySpawn(const TSubclassOf<AEnemyCharacter> EnemyClass, const FVector& SpawnLocation);
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UBoxComponent* SpawnVolume{nullptr};
};
