// Fill out your copyright notice in the Description page of Project Settings.

/* Header for this implementation */
#include "SMagicProjectile.h"

/* Non-engine headers */
#include "SAttributeComponent.h"

/* Engine headers */
#include <Components/SphereComponent.h>



ASMagicProjectile::ASMagicProjectile()
{
	Damage = 20.0f;
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
	if (OtherActor && OtherActor != GetInstigator())
	{
		if (USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass())))
		{
			/* Decrease the health of the OtherActor by the amount of damage this projectile is supposed to do. */
			AttributeComp->ApplyHealthChange(GetInstigator(), -Damage);

			/* Can destroy this magic projectile now... */
			Explode();
		}
	}
}
