// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMagicProjectileBase.h"
#include "STeleportProjectile.generated.h"

/**
 * The instigator fires this projectile and it travels in a straight line,
 * continuing for 2 seconds, or until it hits something, at which point it
 * explodes. Soon after exploding, the Instigator is teleported to the 
 * impact point.
 */
UCLASS()
class ACTIONROGUELIKE_API ASTeleportProjectile : public ASMagicProjectileBase
{
	GENERATED_BODY()
	
public:

	ASTeleportProjectile();

protected:

	virtual void BeginPlay() override;

private:

	/* Timer handle for the timer that governs how long this projectile lives for. */
	FTimerHandle LifeTimeTimer;

	/* How long this projectile lives for (if it doesn't collide with anything) */
	UPROPERTY(EditDefaultsOnly)
	float LifeTime;

	/* Timer handle for the timer that governs how long we wait before teleporting,
	 * after the projectile has exploded. */
	FTimerHandle TeleportWaitTimer;

	/* How long to wait before teleporting after the projectile has exploded */
	UPROPERTY(EditDefaultsOnly)
	float TeleportWaitTime;

	void Explode();

	void TeleportInstigator();

	/* Reference to the sound asset to play when we teleport. Set via the editor. */
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* TeleportSound;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ExplosionEffect;

	/* When this projectile hits something, we want to explode prematurely */
	virtual void NotifyHit(
		class UPrimitiveComponent* MyComp,
		AActor* Other,
		class UPrimitiveComponent* OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;
};
