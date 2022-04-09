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

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void Explode();

	/* The collision geometry for the projectile itself */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;

	/* The effect for the projectile itself */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UParticleSystemComponent* EffectComp;

	/* The movement component that powers the projectile movement */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* ImpactEffect;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UAudioComponent* InFlightAudio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Flight")
	float Speed = 1000.0f;

private:	
	
};
