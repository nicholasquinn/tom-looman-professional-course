// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckInRange.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTService_CheckInRange : public UBTService
{
	GENERATED_BODY()

public:

	USBTService_CheckInRange();

protected:

	UPROPERTY(EditAnywhere, Category = "AI|Services|CheckInRange")
	float Range;

	/* FBlackboardKeySelector is basically the drop-down menu that you see in the editor. 
	 * It is better than putting an FNAME here that is exposed as a UPROPERTY because then
	 * it's possible to type a key name that doesn't exist. With the drop-down, it enforces
	 * you to select a key that definitely exists, kind of like an enum. Furthermore, using
	 * a key selector allows you to change the name of the key in the blackboard without
	 * having to recompile the C++ code. */
	UPROPERTY(EditAnywhere, Category = "AI|Services|CheckInRange")
	FBlackboardKeySelector InRangeKeySelector;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:


};
