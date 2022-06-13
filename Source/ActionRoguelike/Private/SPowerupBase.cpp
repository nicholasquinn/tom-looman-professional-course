// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Net/UnrealNetwork.h>


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

	/* In ctor, so set directly as per log warning inside SetReplicates() */
	bReplicates = true;
}

void ASPowerupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	/* To be Implemented in child classes */
	unimplemented();
}

void ASPowerupBase::OnRep_UpdateState()
{
	RootComponent->SetVisibility(!bConsumed, true);
	SetActorEnableCollision(!bConsumed);
	if (bConsumed) { UGameplayStatics::PlaySoundAtLocation(this, ConsumeSound, GetActorLocation()); }
}

void ASPowerupBase::ConsumePowerup_Implementation()
{
	if (!bConsumed)
	{
		bConsumed = true;
		OnRep_UpdateState();
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASPowerupBase::EnablePowerup, CooldownDuration);
	}
}

void ASPowerupBase::EnablePowerup_Implementation()
{
	bConsumed = false;
	OnRep_UpdateState();
}

void ASPowerupBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupBase, bConsumed);
}