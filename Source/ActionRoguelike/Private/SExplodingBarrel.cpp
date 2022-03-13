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
	MeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

	/* We could have set the impulse to be a change in velocity, which means it ignores mass. 
	 * This means it would send the character and a small box the same distance. Nice to know,
	 * but don't need to do it. */
	RadialForceComp->Radius = 1000.0f; // 10 meter explosion radius
	RadialForceComp->ImpulseStrength = 200000.0f; // tested in editor and seems reasonable
	RadialForceComp->SetAutoActivate(false); // prevent constant force being emitted
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

	/* Note that we could have bound to OnActorHit to run when any component is hit,
	 * rather than a specific one like the barrel mesh. See FActorHitSignature OnActorHit;
	 * at line 890 in Actor.h. Another alternative is to override the NotifyHit function,
	 * which is the function that is bound to the previously mentioned OnActorHit event,
	 * also in Actor.h, or even implement the blueprint handler that it calls, ReceiveHit. */
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
	 * specifically stating it must be an ASMagicProjectile hit that causes explosion, and not a SCharacter hit. 
	 * UPDATE: The course solution goes against this, and Tom stated it's to prevent coupling. When you do casts
	 * you're locking in the dependency type. This may be okay depending on your situation, and you an use
	 * things like interfaces and TSubClassOf etc. to make it a little more flexible, but you're still depending
	 * on some concrete thing. The alternative to this, which is covered later in the course, is to markup classes
	 * with Gameplay Tags, which are basically tags/labels/names that can be added to any Actor. This allows you
	 * to depend on the tag, and Actors can be added and removed from that tag group freely, with no change 
	 * required to the code. Of course, tags don't enforce any sort of interface, so if you do need to call some
	 * method etc, then you may still need to depend on an interface etc. 
	 * Leaving this implementation here but commenting it out. */

	/*
	if (Cast<ASMagicProjectile>(OtherActor) != nullptr)
	{
		RadialForceComp->FireImpulse();
	}
	*/

	/* According to assignment solution, we fire on any collision, even if that's just the player
	 * bumping into the explosive barrel. */
	RadialForceComp->FireImpulse();
}

// Called every frame
void ASExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

