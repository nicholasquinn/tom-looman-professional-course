// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffectThorns.h"
#include <SActionComponent.h>
#include "SAttributeComponent.h"

USActionEffectThorns::USActionEffectThorns()
{
	Duration = 30.0f;
	DamageReflectionFraction = 1.0f / 4; // reflect 1/4 of the damage by default
}

void USActionEffectThorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	UE_LOG(LogTemp, Warning, TEXT("Starting Thorns effect"));

	/* The following are all assumed to be present and valid on an actor that receives this
	 * action effect, but we will explicitly check them and early exit. To prevent silent
	 * failures, they are all ensured. */

	USActionComponent* OwningActionComp = GetOwningComponent();
	if (!ensure(OwningActionComp)) { return; }

	AActor* OwningActor = OwningActionComp->GetOwner();
	if (!ensure(OwningActor)) { return; }

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(OwningActor);
	if (!ensure(AttributeComp)) { return; }

	/* Bind to the attribute component's on health changed event so that we can be notified
	 * when we were damaged, and therefore reflect back some damage. */
	AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffectThorns::OnOwningActorHealthChange);
}

void USActionEffectThorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
	UE_LOG(LogTemp, Warning, TEXT("Stopping Thorns effect"));

	/* Considering start succeeded, we'll just assume this succeeds */
	USAttributeComponent::GetAttributeComponent(
		GetOwningComponent()->GetOwner()
	)->OnHealthChanged.RemoveDynamic(this, &USActionEffectThorns::OnOwningActorHealthChange);
}

void USActionEffectThorns::OnOwningActorHealthChange(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta)
{
	/* Damage the instigator actor based on the amount of damage they dealt to us */

	/* First of all, we're not interested in heals, only damage, specifically from others. */
	if (Delta >= 0.0f || GetOwningComponent()->GetOwner() == InstigatorActor) return;

	/* Get the attributes component of the Instigator and deal damage to them */
	USAttributeComponent* InstigatorAttributes = USAttributeComponent::GetAttributeComponent(InstigatorActor);
	if (!InstigatorAttributes) return;

	const int32 ReflectedDamage = FMath::RoundToInt(FMath::Abs(Delta) * DamageReflectionFraction);
	InstigatorAttributes->ApplyHealthChange(GetOwningComponent()->GetOwner(), -ReflectedDamage);
}
