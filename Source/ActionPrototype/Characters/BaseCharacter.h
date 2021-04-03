// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UBaseResourceComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentHealthIncreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentHealthDecreased, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHealthIncreased, float, Amount, float, NewMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHealthDecreased, float, Amount, float, NewMaxHealth);

UCLASS(Abstract)
class ACTIONPROTOTYPE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Health")
	bool bIsInvulnerable{false};

	UFUNCTION(BlueprintPure, Category="Character Health")
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintPure, Category="Character Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintPure, Category="Character Health")
	float GetNormalisedHealth() const;
	UFUNCTION(BlueprintCallable, Category="Character Health")
	void IncreaseCurrentHealth(const float Heal, const bool bClampToMax = true);
	UFUNCTION(BlueprintCallable, Category="Character Health")
	void DecreaseCurrentHealth(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser);
	UFUNCTION(BlueprintCallable, Category="Character Health")
	void IncreaseMaxHealth(const float Amount, const bool bClampCurrentValue = true);
	UFUNCTION(BlueprintCallable, Category="Character Health")
	void DecreaseMaxHealth(const float Amount, const bool bClampCurrentValue = true);

	UPROPERTY(BlueprintAssignable, Category="Character Health")
	FOnCurrentHealthIncreased OnCurrentHealthIncreased;
	UPROPERTY(BlueprintAssignable, Category="Character Health")
	FOnCurrentHealthDecreased OnCurrentHealthDecreased;
	UPROPERTY(BlueprintAssignable, Category="Character Health")
	FOnMaxHealthIncreased OnMaxHealthIncreased;
	UPROPERTY(BlueprintAssignable, Category="Character Health")
	FOnMaxHealthDecreased OnMaxHealthDecreased;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Character Health")
	void OnZeroHealth();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UBaseResourceComponent* HealthComponent{nullptr};
	
	UFUNCTION()
	void BroadcastCurrentHealthIncreased(const float Amount, const float CurrentHealth);
	UFUNCTION()
	void BroadcastCurrentHealthDecreased(const float Amount, const float CurrentHealth);
};
