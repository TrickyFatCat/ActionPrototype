// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIModule/Classes/AIController.h"
#include "DrawDebugHelpers.h"

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
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return false;
	}
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	FHitResult HitResult;
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - CurrentLocation;
	DirectionToPlayer.Normalize();
	FVector TargetPoint = CurrentLocation + DirectionToPlayer * AggroDistance;
	
	GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, TargetPoint, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
	const APlayerCharacter* HitActor = Cast<APlayerCharacter>(HitResult.GetActor());
	
	if (HitResult.GetActor() == PlayerCharacter)
	{
		return true;
	}

	return false;
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

	if (DistanceToPlayer > AggroDistance)
	{
		return;
	}

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
				if (bIsFollowingPath)
				{
					EnemyController->StopMovement();
				}
				
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
