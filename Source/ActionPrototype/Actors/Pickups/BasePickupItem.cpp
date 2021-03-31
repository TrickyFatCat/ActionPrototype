// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickupItem.h"
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
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABasePickupItem::TriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ABasePickupItem::TriggerOverlapEnd);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	PickupMesh->SetupAttachment(RootComponent);

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

void ABasePickupItem::SetAnimationSpeed(const float NewAnimationSpeed)
{
	AnimationSpeed = NewAnimationSpeed;
	PickupAnimationTimeline->SetPlayRate(AnimationSpeed);
}

void ABasePickupItem::ProcessPickupEffect()
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
	PickupMesh->SetWorldRotation(NewRotation);
}

void ABasePickupItem::AnimatePickupMesh(const float AnimationProgress) const
{
	AnimateMeshLocation(AnimationProgress);
	AnimateMeshRotation();
}

void ABasePickupItem::TriggerOverlapBegin_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (PickupMainParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, PickupMainParticles, MeshInitialLocation);
	}

	if (PickupSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, MeshInitialLocation);
	}
	
	OnPickup();
	Destroy();
}

void ABasePickupItem::TriggerOverlapEnd_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}
