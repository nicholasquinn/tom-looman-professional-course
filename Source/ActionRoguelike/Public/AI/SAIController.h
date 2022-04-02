// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	class UBehaviorTree* BehaviorTree;

private:

};
