// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lid Mesh"));

	LidMesh->SetupAttachment(BaseMesh);

	OpenPitch = 110.0f;

	bIsOpen = false;

	bReplicates = true; 
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	/* When interacted with, we open/close the lid to the target pitch. */
	bIsOpen = !bIsOpen;
	/* Server needs to call toggle lid manually as rep notifies don't run on the server
	 * automatically. This is because if it's a function that does purely cosmetic things,
	 * then we don't want it slowing down the server processing. */
	OnRep_ToggleLid();
}


void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* For some reason we don't need to call StaticClass here... */
	DOREPLIFETIME(ASItemChest, bIsOpen);
}


void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_ToggleLid();
}

/* Called on clients automatically */
void ASItemChest::OnRep_ToggleLid()
{
	FString LidStatus = bIsOpen ? TEXT("Open") : TEXT("Closed");
	UE_LOG(LogTemp, Warning, TEXT("Lid is now %s"), *LidStatus);
	const float TargetPitch = bIsOpen ? OpenPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0));
}

