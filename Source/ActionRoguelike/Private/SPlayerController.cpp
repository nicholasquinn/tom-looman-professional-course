// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

void ASPlayerController::BeginPlayingState()
{
	/* Call event in our BP_Character's blueprint graph now that playerstate is ready */
	BlueprintBeginPlayingState();
}
