// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIModule/Classes/AIController.h"

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller found"));
	}
}

AEnemyCharacter::AEnemyCharacter()
{
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckDistanceToPlayer();
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

void AEnemyCharacter::ChaseTarget(ABaseCharacter* TargetCharacter)
{
	if (EnemyController == nullptr)
	{
		return;
	}

	if (CurrentState != EEnemyState::Chase)
	{
		CurrentState = EEnemyState::Chase;
	}

	EnemyController->MoveToActor(Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this, 0)), AttackRadius - AttackRadius * 0.5f);
}

void AEnemyCharacter::AttackTarget(ABaseCharacter* TargetCharacter)
{
}

void AEnemyCharacter::CheckDistanceToPlayer()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerCharacter == nullptr)
	{
		return;
	}

	const float DistanceToPlayer = GetDistanceTo(PlayerCharacter);

	if (DistanceToPlayer > AggroRadius)
	{
		if (EnemyController->IsFollowingAPath())
		{
			EnemyController->StopMovement();
		}
		
		if (CurrentState != EEnemyState::Idle)
			CurrentState = EEnemyState::Idle;
		return;
	}

	if (!IsPlayerVisible())
	{
		return;
	}

	if (DistanceToPlayer > AttackRadius && CurrentState != EEnemyState::Chase)
	{
		ChaseTarget(Cast<ABaseCharacter>(PlayerCharacter));
	}
	else if (DistanceToPlayer <= AttackRadius && CurrentState != EEnemyState::Attack)
	{
		CurrentState = EEnemyState::Attack;
		// attack
	}
}
