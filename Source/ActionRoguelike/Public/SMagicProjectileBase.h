// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectileBase.generated.h"

UCLASS(Abstract)
class ACTIONROGUELIKE_API ASMagicProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASMagicProjectileBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* The collision geometry for the projectile itself */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;

	/* The effect for the projectile itself */
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* EffectComp;

	/* The movement component that powers the projectile movement */
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MovementComp;

private:	
	
};
