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

	virtual void PostInitProperties() override;

	virtual void NotifyHit(
		class UPrimitiveComponent* MyComp,
		AActor* Other, 
		class UPrimitiveComponent* OtherComp, 
		bool bSelfMoved, 
		FVector HitLocation, 
		FVector HitNormal, 
		FVector NormalImpulse, 
		const FHitResult& Hit
	) override;

protected:

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintNativeEvent)
	void Explode();

	/* The collision geometry for the projectile itself */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;

	/* The effect for the projectile itself */
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* EffectComp;

	/* The movement component that powers the projectile movement */
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly)
	float Speed = 1000.0f;

private:	
	
};
