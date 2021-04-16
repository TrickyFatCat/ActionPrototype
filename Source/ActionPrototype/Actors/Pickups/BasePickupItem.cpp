// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickupItem.h"

#include "ActionPrototype/Characters/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABasePickupItem::ABasePickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup Collision"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetCanEverAffectNavigation(false);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetGenerateOverlapEvents(false);

	PickupIdleParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Pickup Particle"));
	PickupIdleParticles->SetupAttachment(RootComponent);

	PickupAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Pickup Mesh Animation"));
	PickupAnimationTimeline->SetLooping(true);
}

// Called when the game starts or when spawned
void ABasePickupItem::BeginPlay()
{
	MeshInitialLocation = PickupMesh->GetComponentLocation();

	if (LocationAnimationCurve != nullptr)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("AnimatePickupMesh"));
		PickupAnimationTimeline->AddInterpFloat(LocationAnimationCurve, TimelineProgress);
		PickupAnimationTimeline->SetPlayRate(AnimationSpeed);
		PickupAnimationTimeline->Play();
	}

	Super::BeginPlay();
}

// Called every frame
void ABasePickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePickupItem::ProcessPickup( APlayerCharacter* PlayerCharacter)
{
	if (PickupMainParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, PickupMainParticles, MeshInitialLocation);
	}

	if (PickupSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, MeshInitialLocation);
	}

	ActivatePickupEffect(PlayerCharacter);
	OnPickup();
	Destroy();
}

void ABasePickupItem::SetAnimationSpeed(const float NewAnimationSpeed)
{
	AnimationSpeed = NewAnimationSpeed;
	PickupAnimationTimeline->SetPlayRate(AnimationSpeed);
}

void ABasePickupItem::ActivatePickupEffect(APlayerCharacter* PlayerCharacter)
{
}

void ABasePickupItem::AnimateMeshLocation(const float AnimationProgress) const
{
	const FVector NewLocation = MeshInitialLocation + MeshLocationOffset * AnimationProgress;
	PickupMesh->SetWorldLocation(NewLocation);
}

void ABasePickupItem::AnimateMeshRotation() const
{
	const FRotator CurrentRotation = PickupMesh->GetComponentRotation();
	const FRotator NewRotation = CurrentRotation + MeshRotationSpeed * AnimationSpeed;
	PickupMesh->SetRelativeRotation(NewRotation);
}

void ABasePickupItem::AnimatePickupMesh(const float AnimationProgress) const
{
	AnimateMeshLocation(AnimationProgress);
	AnimateMeshRotation();
}