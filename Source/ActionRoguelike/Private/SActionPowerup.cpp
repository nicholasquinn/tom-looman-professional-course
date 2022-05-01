// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionPowerup.h"
#include "SPowerupBase.h"
#include "SActionComponent.h"
#include "GameFramework/Pawn.h"

void ASActionPowerup::Interact_Implementation(APawn* InstigatorPawn)
{
	ensure(GrantedAction);

	Super::ConsumePowerup();

	if (!InstigatorPawn) { return; }

	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	if (!ActionComp) { return; }

	ActionComp->AddAction(InstigatorPawn, GrantedAction);
}
