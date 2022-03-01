// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include <SGameplayInterface.h>
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/* Set sensible defaults for the query, but these can be overridden in the editor. */
	QueryLength = 200.0f;	// 2m sweep 
	QueryRadius = 15.0f;	// of 30cm diameter sphere
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	/* Initialise parameters for Sweep query */

	TArray<FHitResult> OutHits; // out parameter for all hits detected

	FVector EyeLocation;
	FRotator LookDirection;
	/* Get the location and rotation of the RootComponent, which is the CapsuleComponent I believe */
	GetOwner()->GetActorEyesViewPoint(EyeLocation, LookDirection);
	/* Use the look direction and eyes starting location and query length to calculate the sweep end point */
	FVector QueryEnd = EyeLocation + LookDirection.Vector() * QueryLength;

	/* The settings for the object style query, most importantly, what object types (plural) to look for.
	 * We are only interested in WorldDynamic, but nice to know you can put more. */
	FCollisionObjectQueryParams ObjectQuerySettings;
	ObjectQuerySettings.AddObjectTypesToQuery(ECC_WorldDynamic);

	/* The collision shape to sweep through the world. */
	FCollisionShape SphereCollider;
	SphereCollider.SetSphere(QueryRadius);

	/* Perform the actual sweep. Multi means we can detect multiple collisions, by object type means the query is looking for a specific object
	 * type, which was set to WorldDynamic (see above) */
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(OutHits, EyeLocation, QueryEnd, FQuat::Identity, ObjectQuerySettings, SphereCollider);

	/* For each WorldDynamic object that was collided with by the sweep */
	UE_LOG(LogTemp, Warning, TEXT("USInteractionComponent::PrimaryInteract - Performing Sweep Query for Interactable Actors"));
	bool bInteracted = false;
	for (FHitResult& HitResult : OutHits)
	{
		AActor* HitActor = HitResult.GetActor();
		/* If we haven't yet interacted with anything, try interact */
		if (!bInteracted && HitActor && HitActor->Implements<USGameplayInterface>())
		{
			APawn* InteractingPawn = Cast<APawn>(GetOwner());
			ISGameplayInterface::Execute_Interact(HitActor, InteractingPawn);
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, QueryRadius, 16, FColor::Green, false, 2.0f);
			/* Only want to allow interacting with 1 item per attempt */
			bInteracted = true; 
		}
		/* Log all Actors that are detected by the sweep */
		UE_LOG(LogTemp, Warning, TEXT("   Found %s"), HitActor ? *HitActor->GetHumanReadableName() : *FString("Null Actor"));
	}
}

