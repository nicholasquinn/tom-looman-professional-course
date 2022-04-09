// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupBase.h"
#include "SHealthPowerup.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASHealthPowerup : public ASPowerupBase
{
	GENERATED_BODY()

public:

	ASHealthPowerup();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	/* EditAnywhere so that individual health powerups in the map can have different
	 * amounts of healing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float HealAmount;

private:
	
};
