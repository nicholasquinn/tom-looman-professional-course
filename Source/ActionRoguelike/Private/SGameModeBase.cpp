// Fill out your copyright notice in the Description page of Project Settings.

// This file
#include "SGameModeBase.h"

// My files
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"

// UE files
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"


ASGameModeBase::ASGameModeBase()
{
	BotSpawnInterval = 10.0f;
	DefaultBotSpawnLimit = 10;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	GetWorldTimerManager().SetTimer(SpawnBotTimerHandle, this, &ASGameModeBase::SpawnBotTimerElapsed, BotSpawnInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	/* Moved checking of number of bots spawned to here because it's more efficient to
	 * check whether we can even spawn one before even attempting to run the EQS which
	 * is expensive. */

	/* Check how many bots have already been spawned. We have a limit so it doesn't
	 * get too difficult/chaotic.
	 * NOTE: this could definitely be improved. Since we are spawning the bots here,
	 * we can just store them into an array and then on death remove them from said array,
	 * which would prevent us from having to loop all the actors of the world, which is costly.
	 * However, if bots can be spawned elsewhere, like pillar spawn events in risk of rain 2,
	 * then this system would break down, as it wouldn't know about those bots spawned elsewhere.
	 * This could be fixed by having a spawning manager subsystem, in which all bots register
	 * themselves into upon spawn, and unregister themselves on death. Then we can just check
	 * this centralized subsystem to see how many bots currently exist in the world, and make
	 * decisions based on that. We can also spawn bots in any way we like, from any place we
	 * like, and it will still work because every bot registers/unregisters itself. This
	 * system is arguably better, but we don't need such complexity, and are just doing the most
	 * simple implementation here. */
	int32 NumAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* CurrBot = *It;
		USAttributeComponent* AttributeComp
			= Cast<USAttributeComponent>(CurrBot->GetComponentByClass(USAttributeComponent::StaticClass()));

		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			++NumAliveBots;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Found %d bots"), NumAliveBots);

	/* Does the world have the max amount of bots already? If so, early exit. */
	const int32 MaxAllowedBots =
		(DifficultyCurve) ? DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds) : DefaultBotSpawnLimit;
	if (NumAliveBots >= MaxAllowedBots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max number of bots have been spawned. Alive=%d | Max=%d"), NumAliveBots, MaxAllowedBots);
		return;
	}

	/* We are clear to spawn a bot, so do the EQS query */
	UEnvQueryInstanceBlueprintWrapper* BotQuery
		= UEnvQueryManager::RunEQSQuery(this, GetBotSpawnLocationQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(BotQuery))
	{
		BotQuery->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotQueryCompleted);
	}
}

void ASGameModeBase::OnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	/* If the query returns any result other than success, we consider it a failure. */
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get Bot Spawn Location query failed!"));
		return;
	}

	/* Query succeeded, get results. If there are none, return. */
	TArray<FVector> Locations;
	if (!QueryInstance->GetQueryResultsAsLocations(Locations)) { return; }

	/* We have valid results and aren't at the spawn limit, so spawn the AI at the first result location. */
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(MinionAiClass, Locations[0], FRotator::ZeroRotator, ActorSpawnParams);
	DrawDebugSphere(GetWorld(), Locations[0], 25, 8, FColor::Green, false, BotSpawnInterval);

}