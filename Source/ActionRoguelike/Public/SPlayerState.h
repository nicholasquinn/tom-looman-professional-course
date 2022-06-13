// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, DeltaCredits, int32, NewCreditCount);

class USSaveGame;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ASPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 NumCredits);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 NumCredits);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits();

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveGame);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveGame);

protected:

	// turns out OnRep functions can have a parameter, which will be filled by
	// the engine on replication with the old/previous value
	UFUNCTION() void OnRep_Credits(int32 OldCredits);

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastOnCreditsChanged(int32 NumCredits);

	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 StartingCredits;

	UPROPERTY(ReplicatedUsing="OnRep_Credits", VisibleAnywhere, Category = "Credits")
	int32 Credits;

	UPROPERTY(BlueprintAssignable, Category = "Credits")
	FOnCreditsChanged OnCreditsChanged;

};
