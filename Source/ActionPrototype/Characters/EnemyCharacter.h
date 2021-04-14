// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;
class AAIController;

class UAnimMontage;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|AI")
	float AggroDistance{1024.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|AI")
	float AttackRadius{256.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|AI")
	float ChaseMinDistance{64.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemey|AI")
	float ChaseMaxDistance{128.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Attack")
	TSet<FName> AttackSectionsNames{};

protected:
	bool IsPlayerVisible() const;
	virtual void ProcessCharacterDeath() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|State", meta=(AllowPrivateAccess="true"))
	EEnemyState CurrentState{EEnemyState::Idle};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|AI", meta=(AllowPrivateAccess="true"))
	AAIController* EnemyController{nullptr};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy|Attack", meta=(AllowPrivateAccess="true"))
	UAnimMontage* AttackMontage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Attack", meta=(AllowPrivateAccess="true"))
	float MinAttackDelay{0.5f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Attack", meta=(AllowPrivateAccess="true"))
	float MaxAttackDelay{1.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Attack", meta=(AllowPrivateAccess="true"))
	FTimerHandle AttackDelayHandle{};
	UFUNCTION(BlueprintCallable, Category="Enemy|Attack")
	void StartAttackDelayTimer();
	
	
	void ChasePlayer();
	void AttackPlayer();
	UFUNCTION(BlueprintCallable, Category="Enemy|Attack")
	void ContinueAttacking();

	void ProcessEnemyStates();
};
