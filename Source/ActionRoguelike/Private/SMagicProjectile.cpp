// Fill out your copyright notice in the Description page of Project Settings.

/* Header for this implementation */
#include "SMagicProjectile.h"

/* Non-engine headers */
#include "SAttributeComponent.h"
#include "SMagicProjectileBase.h"

/* Engine headers */
#include <Components/SphereComponent.h>
#include "SGameplayFunctionLibrary.h"
#include "SActionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SActionEffect.h"


ASMagicProjectile::ASMagicProjectile()
{
	Damage = 20.0f;
	bHasBeenReflected = false;
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Bind the overlap of the sphere component to a function that detects health
	 * attribute components on the hit actor and attempts to apply damage. */
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnOverlapTryDamageOtherActor);
}

void ASMagicProjectile::OnOverlapTryDamageOtherActor
(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	/* We can only damage the other actor if it isn't null (obviously), and if it isn't null, 
	 * we also don't want to damage the person who fired the projectile. */
	//if (OtherActor && OtherActor != GetInstigator())
	//{
	//	if (USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass())))
	//	{
	//		/* Decrease the health of the OtherActor by the amount of damage this projectile is supposed to do. */
	//		AttributeComp->ApplyHealthChange(GetInstigator(), -Damage);

	//		/* Can destroy this magic projectile now... */
	//		Explode();
	//	}
	//}

	/* If the other actor is null, or it is the same as who shot the projectile, skip i.e. no friendly fire */
	if (!OtherActor || OtherActor == GetInstigator()) return;

	/* Check if the other actor has an action component that contains a parry tag */
	USActionComponent* ActionComponent = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
	if (!bHasBeenReflected && ActionComponent && ActionComponent->ActiveGameplayTags.HasTag(ParryTag))
	{
		// if it does, reflect the projectile back at them, and don't take any damage
		MovementComp->Velocity = -MovementComp->Velocity;
		SetInstigator(Cast<APawn>(OtherActor));
		bHasBeenReflected = true;
		UE_LOG(LogTemp, Warning, TEXT("Projectile reflected!"));
		return; // by early exiting here, we don't take damage below...
	}

	/* New implementation using the GameplayFunctionLibrary */
	if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, Damage, SweepResult))
	{
		if (ActionComponent)
		{
			for (TSubclassOf<USActionEffect> ActionEffect : ActionEffectClasses)
			{
				ActionComponent->AddAction(GetInstigator(), ActionEffect);
			}
		}
		Explode();
	}
}
