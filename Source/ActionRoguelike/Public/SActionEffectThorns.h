// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffectThorns.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffectThorns : public USActionEffect
{
	GENERATED_BODY()
	

public:

	USActionEffectThorns();

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

protected:

	UFUNCTION()
	void OnOwningActorHealthChange(AActor* InstigatorActor, class USAttributeComponent* OwningComponent, float NewHealth, float Delta);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageReflectionFraction;

private:


};
