// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()
	
public:

	/* This function should be used to create USAction components as it sets
	 * members as well. */
	static USAction* New(
		AActor* OwningActor,
		USActionComponent* OwningActionComponent, 
		TSubclassOf<USAction> ActionClass
	);

	UFUNCTION(BlueprintPure, Category = "Action")
	class USActionComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void StopAction(AActor* Instigator);

	class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure, Category = "Action")
	bool GetIsRunning() const;

	/* Should this action be started immediately when added to an action component? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Action")
	bool bAutoStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	FName ActionName;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:

	/* This action cannot run if any of these tags exist in the action component's active tags container. */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	/* This action adds the following tags to the action component's active tags container. */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(ReplicatedUsing="OnRep_IsRunning")
	bool bIsRunning = false;

	UFUNCTION()
	void OnRep_IsRunning();

	UPROPERTY(Replicated)
	USActionComponent* OwningActionComponent;

private:

};
