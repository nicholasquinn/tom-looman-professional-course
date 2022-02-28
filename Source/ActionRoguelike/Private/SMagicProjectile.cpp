// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Create components */
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Comp"));
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect Comp"));

	/* Setup component hierarchy */
	RootComponent = SphereComp;
	EffectComp->SetupAttachment(SphereComp);

	/* Initialise component members */

	/* Set all collision settings to the projectile preset */
	SphereComp->SetCollisionProfileName(FName("Projectile"));

	/* initial speed, overrides initial velocity (which then is interpreted as initial direction) */
	MovementComp->InitialSpeed = 1000.0f;
	/* whether the initial velocity is in local space */
	MovementComp->bInitialVelocityInLocalSpace = true;
	/* whether this projectile will have its rotation updated each frame to match the direction of its velocity. */
	MovementComp->bRotationFollowsVelocity = true;
	/* Don't let gravity effect the path of the projectile */
	MovementComp->ProjectileGravityScale = 0.0f;
}


// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

