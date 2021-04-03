// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "ActionPrototype/ActorComponents/BaseResourceComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UBaseResourceComponent>(TEXT("HealthComponent"));
}

void ABaseCharacter::BeginPlay()
{
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::DecreaseCurrentHealth);
	HealthComponent->OnCurrentValueIncreased.AddDynamic(this, &ABaseCharacter::BroadcastCurrentHealthIncreased);
	HealthComponent->OnCurrentValueDecreased.AddDynamic(this, &ABaseCharacter::BroadcastCurrentHealthDecreased);
	Super::BeginPlay();
}

void ABaseCharacter::BroadcastCurrentHealthIncreased(const float Amount, const float CurrentHealth)
{
	OnCurrentHealthIncreased.Broadcast(Amount, CurrentHealth);
}

void ABaseCharacter::BroadcastCurrentHealthDecreased(const float Amount, const float CurrentHealth)
{
	OnCurrentHealthDecreased.Broadcast(Amount, CurrentHealth);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ABaseCharacter::GetCurrentHealth() const
{
	return HealthComponent->GetCurrentValue();
}

float ABaseCharacter::GetMaxHealth() const
{
	return HealthComponent->GetMaxValue();
}

float ABaseCharacter::GetNormalisedHealth() const
{
	return HealthComponent->GetNormalizedValue();
}

void ABaseCharacter::IncreaseCurrentHealth(const float Heal, const bool bClampToMax)
{
	HealthComponent->IncreaseValue(Heal, bClampToMax);
}

void ABaseCharacter::DecreaseCurrentHealth(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (bIsInvulnerable && GetCurrentHealth() <= 0)
	{
		return;
	}

	HealthComponent->DecreaseValue(Damage);
	const float CurrentHealth = GetCurrentHealth();

	if (CurrentHealth <= 0.f)
	{
		OnZeroHealth();
	}
}

void ABaseCharacter::IncreaseMaxHealth(const float Amount, const bool bClampCurrentValue)
{
	HealthComponent->IncreaseMaxValue(Amount, bClampCurrentValue);
	OnMaxHealthIncreased.Broadcast(Amount, GetMaxHealth());
}

void ABaseCharacter::DecreaseMaxHealth(const float Amount, const bool bClampCurrentValue)
{
	if (GetMaxHealth() <= 0.f)
	{
		return;
	}

	HealthComponent->DecreaseMaxValue(Amount, bClampCurrentValue);
	OnMaxHealthDecreased.Broadcast(Amount, GetMaxHealth());
}
