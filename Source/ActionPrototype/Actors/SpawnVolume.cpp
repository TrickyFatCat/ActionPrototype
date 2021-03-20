// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Math/TransformCalculus3D.h"
#include "ActionPrototype/Characters/EnemyCharacter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Volume"));
	RootComponent = SpawnVolume;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnVolume::GetRandomPoint() const
{
	const FVector VolumeExtent = SpawnVolume->GetScaledBoxExtent();
	const FVector VolumeOrigin = SpawnVolume->GetComponentLocation();
	const FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(VolumeOrigin, VolumeExtent);
	return Point;
}

AEnemyCharacter* ASpawnVolume::ProcessEnemySpawn(const TSubclassOf<AEnemyCharacter> EnemyClass, const FVector& SpawnLocation)
{
	if (EnemyClass == nullptr)
	{
		return {nullptr};
	}

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return {nullptr};
	}

	const FActorSpawnParameters SpawnParameters;
	AEnemyCharacter* EnemyInstance = World->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	OnEnemySpawned(EnemyInstance);
	return EnemyInstance;
}

