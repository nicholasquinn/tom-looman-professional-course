// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SMagicProjectileBase.h"
#include "SActionProjectile.generated.h"

/**
 * An action that wraps up everything that needs to be done for casting an instance
 * of the given projectile class.
 */
UCLASS()
class ACTIONROGUELIKE_API USActionProjectile : public USAction
{
	GENERATED_BODY()
	
public:

	USActionProjectile();

	virtual void StartAction_Implementation(AActor* Instigator) override;

protected:

	UFUNCTION() // Required to bind to timer delegate with .BindUFunction
	void OnCastTimerElapsed(ACharacter* InstigatorCharacter);

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ASMagicProjectileBase> ProjectileClass;

	/* The line trace distance for aiming all attacks */
	UPROPERTY(EditAnywhere, Category = "Projectile|Config")
	int64 AimTraceDistance;

	UPROPERTY(VisibleAnywhere, Category = "Projectile|Constants")
	FName MuzzleName;

	UPROPERTY(EditAnywhere, Category = "Projectile|Config")
	float CastTimerDuration;
	FTimerHandle CastTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Projectile|Assets")
	class UAnimMontage* CastAnim;

	UPROPERTY(EditAnywhere, Category = "Projectile|Assets")
	class UParticleSystem* CastEffect;

private:

};
