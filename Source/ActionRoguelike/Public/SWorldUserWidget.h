// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

/**
 * The base class for any widget that we want to exist in the world and 
 * be projected to screen. You can also use the Widget Component, but
 * since Tom is doing it this way, I'll follow for the sake of keeping
 * in sync with the course.
 * Definitely look into the Widget Component way and perhaps even
 * implement it in this project for something else so you can compare.
 * Perhaps even just re-implement the health bar for the AI via Widget
 * Component.
 */
UCLASS()
class ACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/* Public as the spawner of the widget needs to set this before adding to viewport so it can
	 * be used in OnConstruct. */
	UPROPERTY(BlueprintReadOnly, Category = "UI", meta=(ExposeOnSpawn=true))
	AActor* OwningActor;

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* The BindWidget meta specifier means that the UMG editor/designer will automatically
	 * bind the widget with the same type and name as this variable, to this variable.
	 * i.e. we will create a SizeBox type widget in the designer and call it 'ParentSizeBox',
	 * and then it will automatically be bound to this variable so we can control it via C++. */
	UPROPERTY(meta = (BindWidget))
	class USizeBox* ParentSizeBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset = FVector::ZeroVector;

private:


};
