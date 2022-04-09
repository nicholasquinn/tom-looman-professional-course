// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPowerup.h"
#include "SAttributeComponent.h"


ASHealthPowerup::ASHealthPowerup()
{
	HealAmount = 50.0f;
}

/* If the InstigatorPawn has an SAttributeComponent, then we will attempt
 * to heal them, otherwise we ignore them. If they have the component but
 * are already at full health we also ignore them. On consumption, an 
 * internal cool down is set. */
void ASHealthPowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	/* We depend on attribute component, not any specific kind of Actor or Pawn */
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	/* Ignore actors who don't have an attribute comp, we cannot heal them */
	if (!AttributeComp) { return; }

	if (AttributeComp->GetHealth() >= AttributeComp->GetMaxHealth()) { return; }

	/* At this point we know we are going to consume the powerup, so use it. The base
	 * implementation takes care of the hiding, disabling of collision, and setting the
	 * cooldown. */
	Super::Interact_Implementation(InstigatorPawn);

	/* Note that we added GetHealth and GetMaxHealth getters specifically so that
	 * we don't just set health directly, it should always be set by the attribute
	 * component itself. It's the responsibility of the attribute component, who 
	 * own the health, to update the health. */
	AttributeComp->ApplyHealthChange(HealAmount);
}
