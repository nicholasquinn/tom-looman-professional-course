// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
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

protected:

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

private:

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);


public: /* exec functions */

	UFUNCTION(Exec)
	void KillAI();

};
