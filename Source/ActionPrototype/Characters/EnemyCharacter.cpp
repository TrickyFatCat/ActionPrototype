// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Components/SphereComponent.h"

void AEnemyCharacter::BeginPlay()
{

}

AEnemyCharacter::AEnemyCharacter()
{
	AggroRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Aggro Radius"));
	AggroRadius->SetupAttachment(GetRootComponent());
	AggroRadius->SetSphereRadius(256.f);
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
}

void AEnemyCharacter::ChaseTarget()
{
}
