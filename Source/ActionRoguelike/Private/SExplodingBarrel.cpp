// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplodingBarrel.h"

#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SMagicProjectile.h"


// Sets default values
ASExplodingBarrel::ASExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Comp"));
	RadialForceComp->SetupAttachment(MeshComp);

	/* The barrel should simulate physics so it explodes and moves around itself. 
	 * The collision profile should also be set to the PhysicsActor preset to reflect this.  */
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName("PhysicsActor");

	RadialForceComp->Radius = 1000.0f; // 10 meter explosion radius
	RadialForceComp->ImpulseStrength = 200000.0f; // tested in editor and seems reasonable
}

// Called when the game starts or when spawned
void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASExplodingBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Bind the delegate here because constructor binding doesn't always work, and we
	 * want it to be bound and ready by the time BeginPlay is called. Note that 
	 * AddDynamic is just a macro for internal add dynamic. */
	MeshComp->OnComponentHit.AddDynamic(this, &ASExplodingBarrel::Explode);
}

void ASExplodingBarrel::Explode(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	/* The assignment spec states "When hit by a projectile, applies force to nearby physics simulating Actors.",
	 * specifically stating it must be an ASMagicProjectile hit that causes explosion, and not a SCharacter hit. */
	if (Cast<ASMagicProjectile>(OtherActor) != nullptr)
	{
		RadialForceComp->FireImpulse();
	}
}

// Called every frame
void ASExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

