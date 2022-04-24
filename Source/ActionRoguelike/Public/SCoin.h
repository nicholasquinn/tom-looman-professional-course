// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupBase.h"
#include "SCoin.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASCoin : public ASPowerupBase
{
	GENERATED_BODY()

public:

	ASCoin();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credits")
	int32 NumCreditsToGrant;

};
