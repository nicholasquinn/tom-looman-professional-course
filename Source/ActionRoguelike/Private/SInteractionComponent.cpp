// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include <SGameplayInterface.h>
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.h"


static TAutoConsoleVariable<bool> CVar_DrawDebug(
	TEXT("su.DrawDebug"),
	false,
	TEXT("Turns on drawing of debug helpers."),
	ECVF_Cheat
);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/* Set sensible defaults for the query, but these can be overridden in the editor. */
	QueryLength = 500.0f;	// 5m sweep now that we are going from camera which is further behind player 
	QueryRadius = 15.0f;	// of 30cm diameter sphere
	QueryFrequency = 0.3;
	QueryCollisionChannel = ECollisionChannel::ECC_WorldDynamic;
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Set a periodic timer to perform the line trace for interactables in front of you */
	FTimerHandle TimerHandle_Unused;
	FTimerDelegate Delegate; 
	Delegate.BindUFunction(this, "FindBestInteractable");
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Unused, Delegate, QueryFrequency, true);
}

// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USInteractionComponent::FindBestInteractable()
{
	/* Initialise parameters for Sweep query */

	TArray<FHitResult> OutHits; // out parameter for all hits detected

	FVector EyeLocation;
	FRotator LookDirection;
	/* Get the location and rotation of the RootComponent, which is the CapsuleComponent I believe */
	GetOwner()->GetActorEyesViewPoint(EyeLocation, LookDirection);
	/* Use the look direction and eyes starting location and query length to calculate the sweep end point */
	FVector QueryEnd = EyeLocation + LookDirection.Vector().GetSafeNormal() * QueryLength;

	/* The settings for the object style query, most importantly, what object types (plural) to look for.
	 * We are only interested in WorldDynamic, but nice to know you can put more. */
	FCollisionObjectQueryParams ObjectQuerySettings;
	ObjectQuerySettings.AddObjectTypesToQuery(QueryCollisionChannel);

	/* The collision shape to sweep through the world. */
	FCollisionShape SphereCollider;
	SphereCollider.SetSphere(QueryRadius);

	/* Perform the actual sweep. Multi means we can detect multiple collisions, by object type means the query is looking for a specific object
	 * type, which was set to WorldDynamic (see above) */
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(OutHits, EyeLocation, QueryEnd, FQuat::Identity, ObjectQuerySettings, SphereCollider);

	if (CVar_DrawDebug.GetValueOnGameThread()) {
		DrawDebugLine(GetWorld(), EyeLocation, QueryEnd, FColor::Orange, false, 1.0f);
		DrawDebugSphere(GetWorld(), EyeLocation, QueryRadius, 8, FColor::Orange, false, 1.0f);
		DrawDebugSphere(GetWorld(), QueryEnd, QueryRadius, 8, FColor::Orange, false, 1.0f);
	}

	FocusedActor = nullptr;

	/* For each WorldDynamic object that was collided with by the sweep */
	UE_LOG(LogTemp, Warning, TEXT("USInteractionComponent::PrimaryInteract - Performing Sweep Query for Interactable Actors"));
	bool bInteracted = false;
	for (FHitResult& HitResult : OutHits)
	{
		AActor* HitActor = HitResult.GetActor();
		/* If we haven't yet interacted with anything, try interact */
		if (!bInteracted && HitActor && HitActor->Implements<USGameplayInterface>())
		{
			
			if (CVar_DrawDebug.GetValueOnGameThread()) {
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, QueryRadius, 16, FColor::Green, false, 2.0f);
			}
			FocusedActor = HitActor;
			/* Only want to allow interacting with 1 item per attempt */
			bInteracted = true;
		}

		if (CVar_DrawDebug.GetValueOnGameThread()) { DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, QueryRadius, 16, FColor::Red, false, 2.0f); }

		/* Log all Actors that are detected by the sweep */
		UE_LOG(LogTemp, Warning, TEXT("   Found %s"), HitActor ? *HitActor->GetHumanReadableName() : *FString("Null Actor"));
	}

	/* If we have focus on an interactable */
	if (FocusedActor)
	{
		/* and we haven't yet spawned a widget (and we do have a widget class that we can spawn) */
		if (!DefaultWidget && ensure(DefaultWidgetClass))
		{
			/* Spawn and add it to viewport lazily */
			DefaultWidget = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);

		}

		/* Regardless of whether it was just created or not, set the owning actor and add to viewport if not already there */
		if (DefaultWidget)
		{
			DefaultWidget->OwningActor = FocusedActor;
			if (!DefaultWidget->IsInViewport())
			{
				DefaultWidget->AddToViewport();
			}
		}
	}
	else if (DefaultWidget && DefaultWidget->IsInViewport()) // No focused actor, but we do have a widget
	{
		DefaultWidget->RemoveFromParent();
	}
}

void USInteractionComponent::PrimaryInteract()
{
	if (!FocusedActor) { return; }
	APawn* InteractingPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(FocusedActor, InteractingPawn);
}

