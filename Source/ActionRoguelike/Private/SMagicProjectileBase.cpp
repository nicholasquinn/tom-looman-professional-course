// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASMagicProjectileBase::ASMagicProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Create components */
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Comp"));
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect Comp"));
	InFlightAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("InFlightAudio"));

	/* Setup component hierarchy */
	RootComponent = SphereComp;
	EffectComp->SetupAttachment(SphereComp);
	InFlightAudio->SetupAttachment(SphereComp);

	/* Initialise component members */

	/* Set all collision settings to the projectile preset */
	SphereComp->SetCollisionProfileName(FName("Projectile"));
	SphereComp->SetSphereRadius(10);

	/* initial speed, overrides initial velocity (which then is interpreted as initial direction) */
	MovementComp->InitialSpeed = Speed = 1000.0f;
	/* whether the initial velocity is in local space */
	MovementComp->bInitialVelocityInLocalSpace = true;
	/* whether this projectile will have its rotation updated each frame to match the direction of its velocity. */
	MovementComp->bRotationFollowsVelocity = true;
	/* Don't let gravity effect the path of the projectile */
	MovementComp->ProjectileGravityScale = 0.0f;

	/* Auto activate the in-flight audio component */
	InFlightAudio->bAutoActivate = true;

	/* Camera shake radius */
	ImpactShakeInnerRadius = 100;
	ImpactShakeOuterRadius = 1000;

	//SetReplicates(true);
}

void ASMagicProjectileBase::PostInitProperties()
{
	Super::PostInitProperties();

	/* Both the movement comps initial speed, and the Speed member were defaulted to 1000 in the ctor,
	 * however, it is an exposed property, and so we need to set it here in case it was tweaked in the
	 * editor. Note we could just set it here once, and not have the ctor line, but ill keep both
	 * just for clarity. */
	MovementComp->InitialSpeed = Speed;
}

void ASMagicProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Try get the instigator so we can ignore all collisions from one another */
	if (APawn* InstigatorPawn = GetInstigator())
	{
		/* Tells this component whether to ignore collision with all components of a specific Actor when this component is moved.
		 * Components on the other Actor may also need to be told to do the same when they move.
		 * Does not affect movement of this component when simulating physics.
		 * Basically, tell the projectile to ignore collisions with whomever shot it. */
		SphereComp->IgnoreActorWhenMoving(InstigatorPawn, true);
		/* As stated above, may need to tell the Instigator to ignore the projectile. */
		InstigatorPawn->MoveIgnoreActorAdd(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s:%d - InstigatorPawn was NULL"), TEXT(__FILE__), __LINE__);
	}
}


void ASMagicProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASMagicProjectileBase::Explode_Implementation()
{
	if (ensure(!IsPendingKill()))
	{
		ensure(ImpactEffect); ensure(ImpactSound);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);
		Destroy();
	}
}

void ASMagicProjectileBase::NotifyHit(
	class UPrimitiveComponent* MyComp, 
	AActor* Other, 
	class UPrimitiveComponent* OtherComp, 
	bool bSelfMoved, 
	FVector HitLocation, 
	FVector HitNormal, 
	FVector NormalImpulse, 
	const FHitResult& Hit
)
{
	Explode();
}

