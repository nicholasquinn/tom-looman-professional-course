// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.generated.h"

class USAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USActionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* The tags that indicate the current state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(AActor* InstigatorActor, TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(USAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<USAction*> Actions;

	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TSubclassOf<USAction>> DefaultActions;

private:	

};
