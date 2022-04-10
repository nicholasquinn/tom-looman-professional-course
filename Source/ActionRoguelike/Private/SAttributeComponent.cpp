// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
	: MaxHealth{100}
{
	/* Sensible default for health */
	Health = 100.0f;
}


bool USAttributeComponent::ApplyHealthChange(float DeltaHealth)
{
	const float OldHealth = Health;
	Health = FMath::Clamp(Health += DeltaHealth, 0.0f, MaxHealth);
	/* The true health difference is the difference between where we are now,
	 * and where we were before. */
	const float TrueDelta = Health - OldHealth;

	/* It is safe to call Broadcast on Multicast Delegates, even if no 
	 * functions have been bound. Note that we broadcast AFTER health
	 * has been updated, which is expected by anyone who binds to this. */
	OnHealthChanged.Broadcast(nullptr, this, Health, TrueDelta);

	/* Now we know the true delta, we can return whether we were 
	 * actually healed or damaged */
	return TrueDelta != 0.0f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}
