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

	CooldownDuration = 10;
}

void ASPowerupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASPowerupBase::EnablePowerup, CooldownDuration);
	DisablePowerup();

	UGameplayStatics::PlaySoundAtLocation(this, ConsumeSound, GetActorLocation());
}

void ASPowerupBase::DisablePowerup_Implementation()
{
	MeshComp->SetVisibility(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASPowerupBase::EnablePowerup_Implementation()
{
	MeshComp->SetVisibility(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
