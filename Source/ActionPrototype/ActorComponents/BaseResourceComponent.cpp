// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResourceComponent.h"

// Sets default values for this component's properties
UBaseResourceComponent::UBaseResourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseResourceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBaseResourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBaseResourceComponent::SetMaxValue(const float NewMaxValue)
{
	MaxValue = NewMaxValue;
}

void UBaseResourceComponent::IncreaseValue(const float Amount, const bool bClampToMax)
{
	CurrentValue += Amount;

	if (bClampToMax)
	{
		CurrentValue = FMath::Min(CurrentValue, MaxValue);
	}
}

void UBaseResourceComponent::DecreaseValue(const float Amount)
{
	CurrentValue -= Amount;
	CurrentValue = FMath::Max(CurrentValue, 0.f);
}

void UBaseResourceComponent::IncreaseMaxValue(const float Amount, const bool bClampCurrentValue)
{
	MaxValue += Amount;

	if (bClampCurrentValue)
	{
		CurrentValue = MaxValue;
	}
}

void UBaseResourceComponent::DecreaseMaxValue(const float Amount, const bool bClampCurrentValue)
{
	MaxValue -= Amount;
	MaxValue = FMath::Max(MaxValue, 0.f);

	if (bClampCurrentValue && CurrentValue > MaxValue)
	{
		CurrentValue = MaxValue;
	}
}

float UBaseResourceComponent::GetNormalizedValue() const
{
	return MaxValue > 0.f ? CurrentValue / MaxValue : 0.f;
}

float UBaseResourceComponent::SetRestoreFrequency(float NewRestoreFrequency)
{
	if (NewRestoreFrequency <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid value %f of NewRestoreFrequency in %s of %s."), NewRestoreFrequency, *this->GetName(), *GetOwner()->GetName());
		NewRestoreFrequency = 1.f;
	}

	RestoreFrequency = NewRestoreFrequency;
	RestoreDelayTime = 1.f / RestoreFrequency;
	return RestoreDelayTime;
}

void UBaseResourceComponent::StartAutoRestore()
{
}

