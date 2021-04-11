// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIModule/Classes/AIController.h"

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AAIController>(GetController());
}

AEnemyCharacter::AEnemyCharacter()
{
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ProcessEnemyStates();
}

bool AEnemyCharacter::IsPlayerVisible()
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return false;
	}

	FHitResult TraceHitResult;
	const FCollisionQueryParams TraceCollisionParams = FCollisionQueryParams(FName(""), false, this);
	return GetWorld()->LineTraceSingleByObjectType(
												   TraceHitResult,
												   this->GetActorLocation(),
												   PlayerCharacter->GetActorLocation(),
												   FCollisionObjectQueryParams(
																			   ECollisionChannel::ECC_GameTraceChannel1
																			  ),
												   TraceCollisionParams
												  );
}

void AEnemyCharacter::ChasePlayer()
{
	AActor* PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (EnemyController == nullptr || PlayerActor == nullptr)
	{
		return;
	}

	if (CurrentState != EEnemyState::Chase)
	{
		CurrentState = EEnemyState::Chase;
	}

	const float TargetDistance = FMath::FRandRange(ChaseMinDistance, ChaseMaxDistance);
	EnemyController->MoveToActor(PlayerActor, TargetDistance);
}

void AEnemyCharacter::AttackPlayer()
{
	if (CurrentState != EEnemyState::Attack)
	{
		CurrentState = EEnemyState::Attack;
	}
}

void AEnemyCharacter::ProcessEnemyStates()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	const float DistanceToPlayer = GetDistanceTo(PlayerCharacter);
	const bool bIsFollowingPath = EnemyController->IsFollowingAPath();
	const bool bIsPlayerVisible = IsPlayerVisible();

	switch (CurrentState)
	{
		case EEnemyState::Idle:
			if (!bIsPlayerVisible)
			{
				return;
			}
		
			if (DistanceToPlayer < AttackRadius)
			{
				AttackPlayer();	
			}
			else if (DistanceToPlayer < AggroDistance)
			{
				ChasePlayer();
			}
			break;
		case EEnemyState::Chase:
			if (!bIsPlayerVisible)
			{
				if (bIsFollowingPath)
				{
					EnemyController->StopMovement();
				}

				CurrentState = EEnemyState::Idle;
				return;
			}
			if (DistanceToPlayer < AttackRadius)
			{
				AttackPlayer();
			}
			else if (DistanceToPlayer < AggroDistance)
			{
				ChasePlayer();
			}
			else
			{
				CurrentState = EEnemyState::Idle;

				if (bIsFollowingPath)
				{
					EnemyController->StopMovement();
				}
			}
			break;
		case EEnemyState::Attack:
			if (!bIsPlayerVisible)
			{
				CurrentState = EEnemyState::Idle;
				return;
			}
			if (DistanceToPlayer < AttackRadius)
			{
				AttackPlayer();
			}
			else if (DistanceToPlayer < AggroDistance)
			{
				ChasePlayer();
			}
			else
			{
				CurrentState = EEnemyState::Idle;
			}
			break;
	}
}
