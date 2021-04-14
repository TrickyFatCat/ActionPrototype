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

	if (EnemyController == nullptr)
	{
		this->SpawnDefaultController();
		EnemyController = Cast<AAIController>(GetController());
	}
}

AEnemyCharacter::AEnemyCharacter()
{
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ProcessEnemyStates();
}

bool AEnemyCharacter::IsPlayerVisible() const
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

	GetWorld()->LineTraceSingleByChannel(
										 HitResult,
										 CurrentLocation,
										 TargetPoint,
										 ECollisionChannel::ECC_Visibility,
										 CollisionQueryParams
										);

	if (HitResult.GetActor() == PlayerCharacter)
	{
		return true;
	}

	return false;
}

void AEnemyCharacter::StartAttackDelayTimer()
{
	const float DelayTimer = FMath::FRandRange(MinAttackDelay, MaxAttackDelay);
	GetWorld()->GetTimerManager().SetTimer(
										   AttackDelayHandle,
										   this,
										   &AEnemyCharacter::ContinueAttacking,
										   DelayTimer,
										   false
										  );
}

void AEnemyCharacter::ChasePlayer()
{
	AActor* PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (EnemyController == nullptr || PlayerActor == nullptr)
	{
		return;
	}

	if (Cast<APlayerCharacter>(PlayerActor)->GetCurrentHealth() <= 0.f)
	{
		CurrentState = EEnemyState::Idle;
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
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	if (PlayerCharacter->GetCurrentHealth() <= 0.f)
	{
		CurrentState = EEnemyState::Idle;
	}

	if (CurrentState != EEnemyState::Attack)
	{
		CurrentState = EEnemyState::Attack;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr && AttackMontage != nullptr)
	{
		if (AttackSectionsNames.Num() == 0)
		{
			return;
		}

		if (PlayerCharacter->GetCurrentHealth() <= 0.f)
		{
			AnimInstance->StopAllMontages(0.f);
			return;
		}

		const int32 AnimMontageSection = FMath::RandRange(0, 2);
		const FName SectionName = AttackSectionsNames.Array()[AnimMontageSection];
		AnimInstance->Montage_Play(AttackMontage, 1.f);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}

	// Terrible solution, but the better one is overkill for this project
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Normalize();
	SetActorRotation(DirectionToPlayer.Rotation());
}

void AEnemyCharacter::ContinueAttacking()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	if (PlayerCharacter->GetCurrentHealth() <= 0.f)
	{
		CurrentState = EEnemyState::Idle;
		return;
	}

	const float DistanceToPlayer = GetDistanceTo(PlayerCharacter);

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
}

void AEnemyCharacter::ProcessEnemyStates()
{
	if (EnemyController == nullptr)
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	if (PlayerCharacter->GetCurrentHealth() <= 0.f)
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
			if (!bIsPlayerVisible || PlayerCharacter->GetCurrentHealth() <= 0.f)
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
			break;
	}
}