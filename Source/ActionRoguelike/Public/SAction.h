// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"


/* A struct to bundle all the replication data into a single packet. This is useful
 * as it guarantees all fields are up to date when an update event comes in as
 * the struct as a whole is replicated all at once. */
USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

	/* Do not need to add Replicated keyword to the UPROPERTY specifiers here.
	 * The struct variable declared in USAction below, RepData, instead is what
	 * has the replicated annotation. All fields within a struct are replicated
	 * by default when the struct itself is replicated. You would have to add
	 * NotReplicated to the fields below to explicitly tell them not to. */

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;

	/* This action cannot run if any of these tags exist in the action component's active tags container. */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	/* This action adds the following tags to the action component's active tags container. */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;

	UFUNCTION()
	void OnRep_RepData();

	UPROPERTY(Replicated)
	USActionComponent* OwningActionComponent;

private:

};
