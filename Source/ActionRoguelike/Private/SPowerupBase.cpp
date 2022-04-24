// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASPowerupBase::ASPowerupBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));

	MeshComp->SetupAttachment(RootComponent);
	SphereComp->SetupAttachment(MeshComp);

	/* We are using a dedicated sphere component for collision, so turn off mesh collision. */
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CooldownDuration = 10;
}

void ASPowerupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	/* To be Implemented in child classes */
	unimplemented();
}

void ASPowerupBase::ConsumePowerup_Implementation()
{
	if (!bConsumed)
	{
		bConsumed = true;
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASPowerupBase::EnablePowerup, CooldownDuration);
		DisablePowerup();

		UGameplayStatics::PlaySoundAtLocation(this, ConsumeSound, GetActorLocation());
	}
}

void ASPowerupBase::DisablePowerup_Implementation()
{
	/* It's best to set visibility and propagate, considering we want to show/hide the whole actor. */
	RootComponent->SetVisibility(false, true);
	/* You can actually set collision for the actor as a whole */
	//SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorEnableCollision(false);
}

void ASPowerupBase::EnablePowerup_Implementation()
{
	RootComponent->SetVisibility(true, true);
	// SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetActorEnableCollision(true);
}
