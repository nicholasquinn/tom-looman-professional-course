// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

#include "SAttributeComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("Attribute"));

	StaticMeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::Flash);
}

/* Bound to the Attribute Component's OnHealthChanged delegate, hence the signature. */
void ASTargetDummy::Flash(AActor* InstigatorActor, class USAttributeComponent* OwningComponent, float NewHealth, float Delta)
{
	UE_LOG(LogTemp, Warning, TEXT("Target dummy was hit!"))
	/* if it's a damaging hit, then flash the material to indicate that damage has been taken. */
	if (Delta < 0)
	{
		StaticMeshComp->SetScalarParameterValueOnMaterials("HitTime", GetWorld()->GetTimeSeconds());
	}
}

// Called every frame
void ASTargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

