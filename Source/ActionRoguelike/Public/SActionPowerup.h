// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupBase.h"
#include "SAction.h"
#include "SActionPowerup.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASActionPowerup : public ASPowerupBase
{
	GENERATED_BODY()
	
public:

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Action")
	TSubclassOf<USAction> GrantedAction;

private:


};
