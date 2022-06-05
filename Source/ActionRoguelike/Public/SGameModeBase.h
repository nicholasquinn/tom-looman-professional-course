// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "SPowerupBase.h"
#include "SGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASGameModeBase();

	virtual void StartPlay() override;

	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

	/**
	 * Initialize the game.
	 * The GameMode's InitGame() event is called before any other functions (including PreInitializeComponents() )
	 * and is used by the GameMode to initialize parameters and spawn its helper classes.
	 * @warning: this is called before actors' PreInitializeComponents.
	 */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void ReadSaveGame();

	/* Need to override the _Implementation considering it is a blueprint native event. Don't want to override
	 * the thunk that handles calling the correct version of the actual implementation. */
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

protected:

	UPROPERTY()
	class USSaveGame* CurrentSaveGame;

	FString SaveSlotName;

	FTimerHandle SpawnBotTimerHandle;

	UFUNCTION() /* Required for timer bound functions */
	void SpawnBotTimerElapsed();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float BotSpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UEnvQuery* GetBotSpawnLocationQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 DefaultBotSpawnLimit;

	UFUNCTION() /* Required for delegates bound functions */
	void OnBotQueryCompleted(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionAiClass;

	UFUNCTION() /* Required for delegates bound functions */
	void OnPowerupQueryFinished(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	class UEnvQuery* GetPowerupSpawnLocationsQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<ASPowerupBase>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 NumPowerupsToSpawn;

private:

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);


public: /* exec functions */

	UFUNCTION(Exec)
	void KillAI();

};
