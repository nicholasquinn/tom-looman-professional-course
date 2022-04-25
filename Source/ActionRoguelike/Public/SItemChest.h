// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"

#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASItemChest();

	/* Public, as it reflects the same access level as the Interact declaration.
	 * Other Actors need to be able to call Interact on this actor to interact with it. */
	void Interact_Implementation(APawn* InstigatorPawn);

	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/* RepNotify */
	UPROPERTY(ReplicatedUsing = "OnRep_ToggleLid", BlueprintReadOnly)
	bool bIsOpen;

	UFUNCTION()
	void OnRep_ToggleLid();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(EditAnywhere)
	float OpenPitch;

private:

};
