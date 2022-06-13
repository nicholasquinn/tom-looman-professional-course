// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

/* Early check to ensure we have been given our dependencies */
void ASPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PauseMenuInstance = nullptr;
}

void ASPlayerController::TogglePauseMenu()
{
	HidePauseMenu() || ShowPauseMenu();
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	this->InputComponent->BindAction("TogglePauseMenu", EInputEvent::IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
}

void ASPlayerController::BeginPlayingState()
{
	/* Call event in our BP_Character's blueprint graph now that player state is ready */
	BlueprintBeginPlayingState();
}

bool ASPlayerController::ShowPauseMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT(__FUNCTION__));
	ensure(PauseMenuClass);
	if (PauseMenuInstance) { return false; }
	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (!PauseMenuInstance) { return false; }
	PauseMenuInstance->AddToViewport(100);
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	return true;
}

bool ASPlayerController::HidePauseMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT(__FUNCTION__));
	if (!PauseMenuInstance) { return false; }
	PauseMenuInstance->RemoveFromParent();
	PauseMenuInstance = nullptr;
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	return true;
}
