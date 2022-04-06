// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <SMagicProjectileBase.h>
#include "SBTTask_RangedAttack.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ASMagicProjectileBase> ProjectileClass;

private:

	/* Starts this task, should return Succeeded, Failed or InProgress
	 * (use FinishLatentTask() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
