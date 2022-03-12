// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	class UAnimMontage* AttackAnim;

private:	

	void MoveX(float AxisValue);
	void MoveY(float AxisValue);

	/* PrimaryAttack plays an attack animation, then sets a timer that calls PrimaryAttackCallback
	 * when it expires. The better way of doing this is using animation notifies, which is basically
	 * an event that can be raised from an animation and some logic can be bound to it. */
	void PrimaryAttack();
	void PrimaryAttackCallback();
	UPROPERTY(EditAnywhere, Category="Attack")
	float PrimaryAttackTimerDuration;
	FTimerHandle PrimaryAttackTimerHandle;
	UPROPERTY(EditAnywhere)
	int64 AimTraceDistance;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	class USInteractionComponent* InteractionComp;


};
