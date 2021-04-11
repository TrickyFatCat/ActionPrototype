// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;

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

protected:

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* AggroRadius{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|State", meta=(AllowPrivateAccess="true"))
	EEnemyState CurrentState{EEnemyState::Idle};

	UFUNCTION()
	void ChaseTarget();
};
