// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

class UUserWidget;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void SetPawn(APawn* InPawn) override;

	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;

	/* Blueprint exposed helper function to facilitate toggling the pause menu. */
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	virtual void SetupInputComponent() override;

	/** Pawn has been possessed, so changing state to NAME_Playing. Start it walking and begin playing with it. */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

private:

	/* Functions that implement the actual showing and hiding of the pause menu */
	bool ShowPauseMenu();
	bool HidePauseMenu();

};
