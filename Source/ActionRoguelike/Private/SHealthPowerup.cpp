// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPowerup.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"


ASHealthPowerup::ASHealthPowerup()
{
	HealAmount = 50.0f;
	Cost = 20;
}

/* If the InstigatorPawn has an SAttributeComponent, then we will attempt
 * to heal them, otherwise we ignore them. If they have the component but
 * are already at full health we also ignore them. On consumption, an 
 * internal cool down is set. */
void ASHealthPowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	/* If there is no instigator (there should be), we cannot proceed */
	if (!ensure(InstigatorPawn)) { return; }

	/* It should be a player that is interacting with this health pot, not an AI */
	APlayerController* PC = InstigatorPawn->GetController<APlayerController>();
	if (!ensure(PC)) { return; }

	/* Check if they have enough credits to purchase this health powerup */
	ASPlayerState* SPlayerState = PC->GetPlayerState<ASPlayerState>();
	if (!ensure(SPlayerState)) { return; }

	/* We depend on attribute component, not any specific kind of Actor or Pawn */
	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	/* Ignore actors who don't have an attribute comp, we cannot heal them */
	if (!AttributeComp) { return; }

	/* If we are already full health, we cannot be healed */
	if (AttributeComp->IsFullHealth()) { return; }

	/* Check credits after test for full health, otherwise we might spend money but then not get the heal. */
	if (!SPlayerState->RemoveCredits(Cost))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough credits to purchase health potion"));
		return;
	}

	/* At this point we know we are going to consume the powerup, so use it. The base
	 * implementation takes care of the hiding, disabling of collision, and setting the
	 * cooldown. */
	Super::ConsumePowerup();

	/* Note that we added GetHealth and GetMaxHealth getters specifically so that
	 * we don't just set health directly, it should always be set by the attribute
	 * component itself. It's the responsibility of the attribute component, who 
	 * own the health, to update the health. */
	AttributeComp->ApplyHealthChange(this, HealAmount);
}
