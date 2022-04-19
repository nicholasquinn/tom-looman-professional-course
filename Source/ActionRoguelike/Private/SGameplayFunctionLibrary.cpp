// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include <SAttributeComponent.h>


bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(TargetActor);
	if (!AttributeComp) { return false; } // cannot damage an actor that doesn't have an attribute comp

	return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		if (HitComponent && HitComponent->IsSimulatingPhysics(HitResult.BoneName))
		{
			HitComponent->AddImpulseAtLocation(-HitResult.ImpactNormal * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}

		return true;
	}

	return false;
}

