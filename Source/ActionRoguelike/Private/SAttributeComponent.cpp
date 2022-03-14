// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	/* Sensible default for health */
	Health = 100.0f;
}


bool USAttributeComponent::ApplyHealthChange(float DeltaHealth)
{
	Health += DeltaHealth;

	/* It is safe to call Broadcast on Multicast Delegates, even if no 
	 * functions have been bound */
	OnHealthChanged.Broadcast(nullptr, this, Health, DeltaHealth);

	/* For now, just return whether they are still alive */
	return Health > 0.0f;
}
