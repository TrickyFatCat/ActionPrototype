// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ActionPrototype/Interfaces/ReactToInteraction.h"
#include "GameFramework/Actor.h"
#include "BasePickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;
class UTimelineComponent;
class UParticleSystem;
class APlayerCharacter;

UCLASS()
class ACTIONPROTOTYPE_API ABasePickupItem : public AActor, public IReactToInteraction 
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup", meta=(AllowPrivateAccess="true"))
	bool bIsInteractable{false};
	void ProcessInteraction_Implementation(const APlayerCharacter* PlayerCharacter) override;

	UFUNCTION(BlueprintCallable, Category="Pickup")
	void ProcessPickup(const APlayerCharacter* PlayerCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup|Effects")
	UParticleSystem* PickupMainParticles{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup|Effects")
	USoundBase* PickupSound{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup|Animation")
	UCurveFloat* LocationAnimationCurve{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup|Animation")
	FVector MeshLocationOffset{FVector::ZeroVector};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup|Animation")
	FRotator MeshRotationSpeed{FRotator::ZeroRotator};

	UFUNCTION(BlueprintCallable, Category="Pickup|Animation")
	void SetAnimationSpeed(const float NewAnimationSpeed);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup")
	void OnPickup(const APlayerCharacter* PlayerCharacter);

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* TriggerVolume{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickupMesh{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* PickupIdleParticles{nullptr};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UTimelineComponent* PickupAnimationTimeline{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Animation", meta=(AllowPrivateAccess="true"))
	float AnimationSpeed{1.f};

	UPROPERTY(BlueprintReadOnly, Category="Pickup|Mesh", meta=(AllowPrivateAccess="true"))
	FVector MeshInitialLocation{FVector::ZeroVector};

	void AnimateMeshLocation(const float AnimationProgress) const;
	void AnimateMeshRotation() const;
	UFUNCTION()
	void AnimatePickupMesh(const float AnimationProgress) const;
};
