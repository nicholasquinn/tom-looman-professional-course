// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
	: MaxHealth{100}
{
	/* Sensible default for health */
	Health = 100.0f;
}


bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float DeltaHealth)
{
	/* Check if the owning actor is currently invincible. Note that the "God" command will set
	 * bCanBeDamaged to false in the Pawn possessed by the PC. Note however, that because
	 * God is an Exec command, it will not effect AI characters, as they are not possessed by
	 * a PC. This means turning god mode on will allow us to still damage AI, regardless of
	 * the code below. */
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	const float OldHealth = Health;
	Health = FMath::Clamp(Health += DeltaHealth, 0.0f, MaxHealth);
	/* The true health difference is the difference between where we are now,
	 * and where we were before. */
	const float TrueDelta = Health - OldHealth;

	/* It is safe to call Broadcast on Multicast Delegates, even if no 
	 * functions have been bound. Note that we broadcast AFTER health
	 * has been updated, which is expected by anyone who binds to this. */
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, TrueDelta);

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

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	/* It may already be dead */
	return ApplyHealthChange(InstigatorActor, -MaxHealth);
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(AActor* FromActor)
{
	/* IsValid is a helper function for not null and not pending kill */
	return IsValid(FromActor) 
		? Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()))
		: nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(FromActor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	UE_LOG(LogTemp, Warning, TEXT("The supplied actor does not have a attribute component. Therefore returning false for IsAlive."))
	ensure(nullptr); // purposefully want to pause execution here!
	return false;
}