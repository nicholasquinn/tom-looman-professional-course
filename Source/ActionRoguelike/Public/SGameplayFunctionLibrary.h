// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/* Library functions */

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Damage")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount);

	/* Note that the FHitResult reference must be a const reference, otherwise the blueprint node that is generated for this 
	 * function will have it as an output pin as it thinks it's an OUT parameter. */
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Damage")
	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

private:

	/* Helper functions */

};
