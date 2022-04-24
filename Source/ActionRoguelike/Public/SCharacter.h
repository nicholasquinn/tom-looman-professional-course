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

	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Attributes")
	class USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Actions")
	class USActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components | Core")
	class USInteractionComponent* InteractionComp;

private:	

	void MoveX(float AxisValue);
	void MoveY(float AxisValue);

	void StartSprinting();
	void StopSprinting();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, class USAttributeComponent* OwningComponent, float NewHealth, float Delta);

	/* <Foo>Attack plays simply calls a projectile action via the action component. */
	void PrimaryAttack();
	void SecondaryAttack();
	void UltimateAttack();

	UPROPERTY(VisibleAnywhere, Category="Components | Core")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components | Core")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
	FName HitTimeName;

	void DrawControlVsPawnRotationDebugArrows();

public: /* Additional public section specifically for exec functions*/

	UFUNCTION(Exec)
	void AddHealth(float Amount = 100);
};
