// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"


/* Rather than adding a query directly to the SCharacter class, we have created an Actor component
 * that provides this functionality. This is good, because now other classes can do interactions,
 * not just our SCharacter class. It also prevents the SCharacter class from becoming too monolithic.
 * The SCharacter class is for implementing the high-level gameplay behavior of our player, and its
 * implementation should mainly focus on interactions between components at a higher-level. Read
 * the rainbow-OOP page again for more on the Single Responsibility Principle and components. */

/* Set the actor group that the class is show in, in the editor. Make it spawnable in Blueprints. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USInteractionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* This component will be registered to Actors, which need to be able to call this PrimaryInteract functionality.
	 * Therefore, it must be public. */
	void PrimaryInteract();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float QueryLength; // Interaction query distance

	UPROPERTY(EditAnywhere)
	float QueryRadius; // Interaction query radius (using a sphere sweep)

private:
		
};
