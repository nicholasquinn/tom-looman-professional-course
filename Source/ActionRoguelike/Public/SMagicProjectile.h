// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectileBase.h"
#include "GameplayTagContainer.h"
#include "Chaos/Array.h"

#include "SMagicProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public ASMagicProjectileBase
{
	GENERATED_BODY()
	
public:

	ASMagicProjectile();

protected:

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnOverlapTryDamageOtherActor(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class USActionEffect>> ActionEffectClasses;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage;

	bool bHasBeenReflected;

};
