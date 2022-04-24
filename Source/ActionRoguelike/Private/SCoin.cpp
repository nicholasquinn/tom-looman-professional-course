// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoin.h"

#include "SPowerupBase.h"
#include "SPlayerState.h"


ASCoin::ASCoin()
{
	CooldownDuration = 60.0f;
	NumCreditsToGrant = 1.0f;
}

void ASCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	ensureAlways(InstigatorPawn);

	/* Consume */
	Super::ConsumePowerup();

	/* Grant credits to the player who is interacting */
	ASPlayerState* SPlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
	if (SPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding credits to player state"));
		SPlayerState->AddCredits(NumCreditsToGrant);
	}

}
