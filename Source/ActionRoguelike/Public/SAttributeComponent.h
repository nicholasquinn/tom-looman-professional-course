// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

/* As the macro name suggests, this declares a new type of delegate (event), specifically a
 * dynamic (can be serialized and have its bound functions found by name) multicast (can have
 * many functions bound to it, but no return value) delegate with four parameters. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnHealthChanged, AActor*, InstigatorActor, class USAttributeComponent*, OwningComponent, float, NewHealth, float, Delta
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(float DeltaHealth);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	/* This is an actual instance of the FOnHealthChanged delegate/event/broadcaster. */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

private:	

		
};
