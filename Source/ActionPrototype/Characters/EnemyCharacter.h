// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;
class AAIController;

UENUM()
enum class EEnemyState : uint8
{
	Idle,
	Chase,
	Attack,
	Death
};

/**
 * 
 */
UCLASS()
class ACTIONPROTOTYPE_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|State")
	EEnemyState InitialState{EEnemyState::Idle};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float AggroRadius{512.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float AttackRadius{256.f};

protected:
	bool IsPlayerVisible();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|State", meta=(AllowPrivateAccess="true"))
	EEnemyState CurrentState{EEnemyState::Idle};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|AI", meta=(AllowPrivateAccess="true"))
	AAIController* EnemyController{nullptr};

	UFUNCTION()
	void ChaseTarget(ABaseCharacter* TargetCharacter);
	void AttackTarget(ABaseCharacter* TargetCharacter);
	

	void CheckDistanceToPlayer();
};
