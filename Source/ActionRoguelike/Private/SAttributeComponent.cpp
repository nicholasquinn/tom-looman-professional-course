// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVar_DamageMultiplier(
	TEXT("su.DamageMultiplier"),
	1.0f,
	TEXT("Scales the damage dealt in attribute components"),
	ECVF_Cheat
);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
	: Health{100}
	, MaxHealth{100}
	, LowHealthThreshold{ 0.5 }
	, Rage{0.0f}
	, MaxRage{50.0f}
	, DamageToRageFraction {0.2f}
{
	SetIsReplicatedByDefault(true);
}


void USAttributeComponent::ClearEvents()
{
	/* We are dead, clear bindings as we don't want to effect UI. Otherwise, if new pawn spawns
	 * and our dot-damage is still ticking, it will set the UI to have 0 health, even though
	 * the new pawn has full health, because these events are bound to the UI and still ticking
	 * as this instance is from its perspective the old pawn's attribute comp which has 0 HP. */
	OnHealthChanged.Clear();
	OnRageChanged.Clear();
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float DeltaHealth)
{
	/* Check if the owning actor is currently invincible. Note that the "God" command will set
	 * bCanBeDamaged to false in the Pawn possessed by the PC. Note however, that because
	 * God is an Exec command, it will not effect AI characters, as they are not possessed by
	 * a PC. This means turning god mode on will allow us to still damage AI, regardless of
	 * the code below. */
	if (!GetOwner()->CanBeDamaged() && DeltaHealth < 0.0f)
	{
		return false;
	}

	/* Before calculating damage, apply damage multiplier. Note the damage multiplier is only
	 * applied if this is a damaging event */
	if (DeltaHealth < 0.0f)
	{
		DeltaHealth *= CVar_DamageMultiplier.GetValueOnGameThread();
	}

	const float OldHealth = Health;
	const float NewHealth = FMath::Clamp(Health += DeltaHealth, 0.0f, MaxHealth);

	/* The true health difference is the difference between where we are now,
	 * and where we were before. Client needs this to be able to return true/false */
	const float TrueDelta = NewHealth - OldHealth;

	/* Only update the replicated health variable on the server */
	if (InstigatorActor->HasAuthority())
	{
		/* Must store new health in variable above because client needs to also 
		 * know the value for the true delta calculation below. */
		Health = NewHealth;

		/* Now we know the true delta, we can calculate the amount of rage this damage produces */
		if (TrueDelta < 0.0f) // if it's damage, not heal
		{
			const float OldRage = Rage;
			Rage = FMath::Clamp(Rage + FMath::Abs(TrueDelta) * DamageToRageFraction, 0.0f, MaxRage);
			const float RageDelta = Rage - OldRage;
			MulticastOnRageChanged(InstigatorActor, Rage, RageDelta);
		}

		/* Run the broadcast on server (here) and all clients */
		MulticastOnHealthChanged(InstigatorActor, Health, TrueDelta);

		/* If we are at or below 0HP, and we just took damage (negative delta), we are dead */
		if (Health <= 0.0f && TrueDelta < 0.0f)
		{
			/* The attribute component now depends on the ASGameMode. Is there a better
			 * way of doing this? */
			ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GameMode)
			{
				GameMode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	/* Now we know the true delta, we can return whether we were 
	 * actually healed or damaged */
	return TrueDelta != 0.0f;
}


bool USAttributeComponent::ConsumeRage(float RageAmount)
{
	if (RageAmount > Rage) return false;
	Rage -= RageAmount;
	return true;
}

void USAttributeComponent::MulticastOnHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}

bool USAttributeComponent::IsLowHealth() const
{
	return (Health / MaxHealth) <= LowHealthThreshold;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	/* It may already be dead */
	return ApplyHealthChange(InstigatorActor, -MaxHealth);
}


void USAttributeComponent::MulticastOnRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(AActor* FromActor)
{
	/* IsValid is a helper function for not null and not pending kill */
	return IsValid(FromActor) 
		? Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()))
		: nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(FromActor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	UE_LOG(LogTemp, Warning, TEXT("The supplied actor does not have a attribute component. Therefore returning false for IsAlive."))
	ensure(nullptr); // purposefully want to pause execution here!
	return false;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
}