// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

#include "Components/StaticMeshComponent.h"


// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lid Mesh"));

	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110.0f;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	/* When interacted with, we open the lid to the target pitch. */
	LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}

