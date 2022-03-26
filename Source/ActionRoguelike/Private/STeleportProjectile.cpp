// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASTeleportProjectile::ASTeleportProjectile()
{
	/* Initialise members with sensible defaults */
	LifeTime = 0.2;
	TeleportWaitTime = 0.1f;
}

void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	/* We have just been spawned by our instigator, set lifetime timer. */
	GetWorldTimerManager().SetTimer(LifeTimeTimer, this, &ASTeleportProjectile::Explode, LifeTime, false);
}

/* Could have used AddDynamic in PostInitializeComponents to bind to the actual sphere collider's
 * OnComponentHit delegate, but since we just care about this actor in general being hit, we can
 * simply override the NotifyHit method. This also overrides the base projectiles implementation,
 * which calls explode immediately, but we also need to clear the timer for the teleport projectile. */
void ASTeleportProjectile::NotifyHit(
	class UPrimitiveComponent* MyComp,
	AActor* Other,
	class UPrimitiveComponent* OtherComp, 
	bool bSelfMoved, 
	FVector HitLocation, 
	FVector HitNormal, 
	FVector NormalImpulse, 
	const FHitResult& Hit)
{
	/* First, disable the LifeTimeTimer so we don't call Explode twice */
	GetWorldTimerManager().ClearTimer(LifeTimeTimer);

	Explode();
}

/* Called when our projectile's lifetime has come to an end, either as a result of hitting something, 
 * or its lifetime completing. Starts the teleport timer. This overrides the default behaviour of
 * the base projectile. */
void ASTeleportProjectile::Explode_Implementation()
{
	/* We can't actually Destroy() yet, as we still need to wait for the explosion effect to complete
	 * and then teleport the instigator to the current location. For this reason we must; */

	/* Stop movement so we don't teleport them further than this point. */
	MovementComp->StopMovementImmediately();
	/* Set our visibility to false to "destroy" the projectile visually. */
	EffectComp->SetVisibility(false);
	/* Spawn the explosion effect */
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());

	/* Set the timer to teleport to the projectile location */
	GetWorldTimerManager().SetTimer(TeleportWaitTimer, this, &ASTeleportProjectile::TeleportInstigator, TeleportWaitTime, false);
}

void ASTeleportProjectile::TeleportInstigator()
{
	UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, GetActorLocation());
	
	/* Try teleport to the final destination if possible. Will slightly adjust to be able to 
	 * teleport if there are blocking collisions. */
	GetInstigator()->TeleportTo(GetActorLocation(), GetInstigator()->GetActorRotation());

	Destroy();
}
