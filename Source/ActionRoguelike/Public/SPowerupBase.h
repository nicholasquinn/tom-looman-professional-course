// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPowerupBase.generated.h"

UCLASS(Abstract)
class ACTIONROGUELIKE_API ASPowerupBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ASPowerupBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:

	UFUNCTION(BlueprintNativeEvent)
	void ConsumePowerup();

	UFUNCTION(BlueprintNativeEvent)
	void DisablePowerup();

	UFUNCTION(BlueprintNativeEvent)
	void EnablePowerup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
	class USoundBase* ConsumeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	float CooldownDuration;

	FTimerHandle CooldownTimerHandle;

private:	


};
