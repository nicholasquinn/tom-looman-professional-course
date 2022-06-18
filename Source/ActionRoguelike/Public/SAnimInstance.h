// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

/**
 * The C++ parent of Blueprint Animation Graphs.
 * Used for creating custom logic and then you set the blueprint anim graph parent to this
 * in its class settings.
 */
UCLASS()
class ACTIONROGUELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C++")
	bool bIsStunned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++")
	class USActionComponent* ActionComp;

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

private:


};
