// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMinionDataAsset.generated.h"


class USAction;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USMinionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// first param: the type specified in the project settings -> asset manager -> primary assets to scan for -> [primary asset name]
		// second param: the name of this data asset in the content browser (not the class name, but the actual instance name)
		// together these form a unique id
		return FPrimaryAssetId("Minions", GetFName());
	}

protected:



private:
	


};
