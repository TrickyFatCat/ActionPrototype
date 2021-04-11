// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

AEnemyCharacter::AEnemyCharacter()
{
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckDistanceToPlayer();
}

void AEnemyCharacter::ChaseTarget()
{
}

void AEnemyCharacter::CheckDistanceToPlayer()
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	const float DistanceToPlayer = GetDistanceTo(PlayerCharacter);

	if (DistanceToPlayer > AggroRadius)
	{
		CurrentState = EEnemyState::Idle;
		return;
	}

	FHitResult TraceHitResult;
	FCollisionQueryParams TraceCollisionParams = FCollisionQueryParams(FName(""), false, this);
	bool bIsPlayerVisible = GetWorld()->LineTraceSingleByObjectType(
																	TraceHitResult,
																	this->GetActorLocation(),
																	PlayerCharacter->GetActorLocation(),
																	FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1),
																	TraceCollisionParams
																   );

	if (!bIsPlayerVisible)
	{
		return;
	}
	
	if (DistanceToPlayer > AttackRadius)
	{
		CurrentState = EEnemyState::Chase;
		// check sight
		// chase
	}
	else
	{
		CurrentState = EEnemyState::Attack;
		// check sight
		// attack
	}
}
