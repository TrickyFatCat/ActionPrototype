// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "ActionPrototype/ActorComponents/BaseResourceComponent.h"
#include "ActionPrototype/Actors/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UBaseResourceComponent>(TEXT("HealthComponent"));

	LeftWeaponComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("LeftWeapon"));
	LeftWeaponComponent->SetupAttachment(GetMesh());
	RightWeaponComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightWeapon"));
	RightWeaponComponent->SetupAttachment(GetMesh());
}

void ABaseCharacter::EquipWeapon(const TSubclassOf<AWeapon> NewWeapon, const EWeaponSlot WeaponSlot)
{
	switch (WeaponSlot)
	{
		case EWeaponSlot::Left:
			if (LeftWeaponComponent->GetClass() == NewWeapon)
			{
				return;
			}

			LeftWeaponComponent->SetChildActorClass(NewWeapon);
			LeftWeapon = Cast<AWeapon>(LeftWeaponComponent->GetChildActor());

			if (LeftWeapon != nullptr)
			{
				LeftWeapon->SetOwner(this);
			}
			break;
		case EWeaponSlot::Right:
			if (RightWeaponComponent->GetClass() == NewWeapon)
			{
				return;
			}

			RightWeaponComponent->SetChildActorClass(NewWeapon);
			RightWeapon = Cast<AWeapon>(RightWeaponComponent->GetChildActor());

			if (RightWeapon != nullptr)
			{
				RightWeapon->SetOwner(this);
			}
			break;
	}
}

AWeapon* ABaseCharacter::GetLeftWeapon() const
{
	return LeftWeapon;
}

AWeapon* ABaseCharacter::GetRightWeapon() const
{
	return RightWeapon;
}

void ABaseCharacter::BeginPlay()
{
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::DecreaseCurrentHealth);
	HealthComponent->OnCurrentValueIncreased.AddDynamic(this, &ABaseCharacter::BroadcastCurrentHealthIncreased);
	HealthComponent->OnCurrentValueDecreased.AddDynamic(this, &ABaseCharacter::BroadcastCurrentHealthDecreased);

	USkeletalMeshComponent* CharacterMesh = GetMesh();
	
	if (CharacterMesh != nullptr)
	{
		LeftWeaponComponent->AttachToComponent(
											   CharacterMesh,
											   FAttachmentTransformRules::KeepWorldTransform,
											   LeftWeaponSocketName
											  );
		RightWeaponComponent->AttachToComponent(
												CharacterMesh,
												FAttachmentTransformRules::KeepWorldTransform,
												RightWeaponSocketName
											   );
		EquipWeapon(DefaultLeftWeaponClass, EWeaponSlot::Left);
		EquipWeapon(DefaultRightWeaponClass, EWeaponSlot::Right);
	}
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

void ABaseCharacter::SwitchLeftWeaponCollision(const bool bIsEnabled) const
{
	if (LeftWeapon == nullptr)
	{
		return;
	}

	if (bIsEnabled)
	{
		LeftWeapon->EnableCollision();
	}
	else
	{
		LeftWeapon->DisableCollision();
	}
}

void ABaseCharacter::SwitchRightWeaponCollision(const bool bIsEnabled) const
{
	if (RightWeapon == nullptr)
	{
		return;
	}

	if (bIsEnabled)
	{
		RightWeapon->EnableCollision();
	}
	else
	{
		RightWeapon->DisableCollision();
	}
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
