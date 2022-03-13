// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMagicProjectileBase.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SCharacter.generated.h"

class UAnimMontage;


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

	/* The class types for each of our different attacks */

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<ASMagicProjectileBase> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASMagicProjectileBase> SecondaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASMagicProjectileBase> UltimateProjectileClass;

	/* The attack animations for each attack */

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* PrimaryAttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* SecondaryAttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* UltimateAttackAnim;

private:	

	void MoveX(float AxisValue);
	void MoveY(float AxisValue);

	/* This function handles the aiming and spawning of the passed in projectile. */
	void Attack(TSubclassOf<ASMagicProjectileBase> ProjectileTypeToSpawn);

	/* <Foo>Attack plays an attack animation, then sets a timer that calls <Foo>AttackCallback
	 * when it expires. The better way of doing this is using animation notifies, which is basically
	 * an event that can be raised from an animation and some logic can be bound to it. */

	void PrimaryAttack();
	void SecondaryAttack();
	void UltimateAttack();

	/* <Foo>AttackCallback simply calls Attack() and passes in the correct projectile class to
	 * Spawn. This only handles the aiming and spawning of the projectile, everything else
	 * that is custom to each projectile is handled internally in each. */

	void PrimaryAttackCallback();
	void SecondaryAttackCallback();
	void UltimateAttackCallback();

	/* The timer settings for each attack type; configurable duration and handle. */

	UPROPERTY(EditAnywhere, Category="Attack")
	float PrimaryAttackTimerDuration;
	FTimerHandle PrimaryAttackTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float SecondaryAttackTimerDuration;
	FTimerHandle SecondaryAttackTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float UltimateAttackTimerDuration;
	FTimerHandle UltimateAttackTimerHandle;

	/* The line trace distance for aiming all attacks */
	UPROPERTY(EditAnywhere)
	int64 AimTraceDistance;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	class USInteractionComponent* InteractionComp;

	void DrawControlVsPawnRotationDebugArrows();

};
