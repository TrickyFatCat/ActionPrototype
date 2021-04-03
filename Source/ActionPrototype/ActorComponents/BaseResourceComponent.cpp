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

	ChangeDelayTime = 1 / ChangeFrequency;

	if (bCustomInitialValue)
	{
		CurrentValue = InitialValue;
	}

	if (bAutoChange && !IsCurrentValueOutOfBounds())
	{
		StartAutoChange();
	}
	// ...
}


// Called every frame
void UBaseResourceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UBaseResourceComponent::GetCurrentValue() const
{
	return CurrentValue;
}

float UBaseResourceComponent::GetMaxValue() const
{
	return MaxValue;
}

void UBaseResourceComponent::SetMaxValue(const float NewMaxValue)
{
	MaxValue = NewMaxValue;
}

void UBaseResourceComponent::IncreaseValue(const float Amount, const bool bClampToMax)
{
	if (bClampToMax && CurrentValue >= MaxValue)
	{
		return;
	}
	
	CurrentValue += Amount;

	if (bClampToMax)
	{
		CurrentValue = FMath::Min(CurrentValue, MaxValue);
	}

	OnCurrentValueIncreased.Broadcast(Amount, CurrentValue);

	if (bAutoChange && bIsDecreasing)
	{
		ProcessAutoChange();
	}
}

void UBaseResourceComponent::DecreaseValue(const float Amount)
{
	if (CurrentValue <= 0.f)
	{
		return;
	}
	
	CurrentValue -= Amount;
	CurrentValue = FMath::Max(CurrentValue, 0.f);
	OnCurrentValueDecreased.Broadcast(Amount, CurrentValue);

	if (bAutoChange && !bIsDecreasing && CurrentValue > 0.f)
	{
		ProcessAutoChange();
	}
}

void UBaseResourceComponent::IncreaseMaxValue(const float Amount, const bool bClampCurrentValue)
{
	MaxValue += Amount;
	OnMaxValueIncreased.Broadcast(Amount, MaxValue);

	if (bClampCurrentValue)
	{
		CurrentValue = MaxValue;
	}
}

void UBaseResourceComponent::DecreaseMaxValue(const float Amount, const bool bClampCurrentValue)
{
	MaxValue -= Amount;
	MaxValue = FMath::Max(MaxValue, 0.f);
	OnMaxValueDecreased.Broadcast(Amount, MaxValue);

	if (bClampCurrentValue && CurrentValue > MaxValue)
	{
		CurrentValue = MaxValue;
	}
}

float UBaseResourceComponent::GetNormalizedValue() const
{
	return MaxValue > 0.f ? CurrentValue / MaxValue : 0.f;
}

float UBaseResourceComponent::GetThresholdValue() const
{
	return bIsDecreasing ? MaxValue * ChangeMinThreshold : MaxValue * ChangeMaxThreshold;
}

float UBaseResourceComponent::SetRestoreFrequency(float NewRestoreFrequency)
{
	if (NewRestoreFrequency <= 0.f)
	{
		UE_LOG(
			   LogTemp,
			   Error,
			   TEXT("Invalid value %f of NewRestoreFrequency in %s of %s."),
			   NewRestoreFrequency,
			   *this->GetName(),
			   *GetOwner()->GetName()
			  );
		NewRestoreFrequency = 1.f;
	}

	ChangeFrequency = NewRestoreFrequency;
	ChangeDelayTime = 1.f / ChangeFrequency;
	return ChangeDelayTime;
}

void UBaseResourceComponent::StartAutoChange()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (ChangeDelayTime <= 0.f || TimerManager.IsTimerActive(ChangeTimerHandle))
	{
		return;
	}

	TimerManager.SetTimer(ChangeTimerHandle, this, &UBaseResourceComponent::ChangeCurrentValue, ChangeDelayTime, true);
}

void UBaseResourceComponent::StopAutoChange()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(ChangeTimerHandle))
	{
		TimerManager.ClearTimer(ChangeTimerHandle);
	}
}

void UBaseResourceComponent::ChangeCurrentValue()
{
	if (bIsDecreasing)
	{
		DecreaseValue(ChangeAmount);
	}
	else
	{
		IncreaseValue(ChangeAmount);
	}

	if (IsCurrentValueOutOfBounds())
	{
		StopAutoChange();
	}
}

void UBaseResourceComponent::StartDelayTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (ChangeStartDelay <= 0.f || TimerManager.IsTimerActive(ChangeStartDelayHandle))
	{
		return;
	}

	TimerManager.SetTimer(
						  ChangeStartDelayHandle,
						  this,
						  &UBaseResourceComponent::StartAutoChange,
						  ChangeStartDelay,
						  false
						 );
}

void UBaseResourceComponent::StopDelayTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(ChangeStartDelayHandle))
	{
		TimerManager.ClearTimer(ChangeStartDelayHandle);
	}
}

bool UBaseResourceComponent::IsCurrentValueOutOfBounds() const
{
	const float TargetValue = GetThresholdValue();
	return bIsDecreasing ? CurrentValue <= TargetValue : CurrentValue >= TargetValue;
}

void UBaseResourceComponent::ProcessAutoChange()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(ChangeStartDelayHandle))
	{
		StopAutoChange();
	}
	
	if (ChangeStartDelay > 0.f)
	{
		if (TimerManager.IsTimerActive(ChangeStartDelayHandle))
		{
			StopDelayTimer();
		}
		
		StartDelayTimer();
		return;
	}

	if (!TimerManager.IsTimerActive(ChangeTimerHandle))
	{
		StartAutoChange();
	}
}
