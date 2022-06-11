// Fill out your copyright notice in the Description page of Project Settings.

// This file
#include "SGameModeBase.h"

// My files
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"

// UE files
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "SPlayerState.h"
#include "SHealthPowerup.h"
#include "SCoin.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGame.h"
#include "Logging/LogVerbosity.h"
#include "GameFramework/GameStateBase.h"
#include <EngineUtils.h>
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/MemoryWriter.h"
#include "SGameplayInterface.h"


static TAutoConsoleVariable<bool> CVar_SpawnBots(
	TEXT("su.SpawnBots"),
	false,
	TEXT("Determines whether bots will spawn or not."),
	ECVF_Cheat /* Enum Console Variable Flags :: Cheat */
);

ASGameModeBase::ASGameModeBase()
{
	BotSpawnInterval = 10.0f;
	DefaultBotSpawnLimit = 10;
	NumPowerupsToSpawn = 30;

	// Since we aren't making a blueprint child of ASPlayerState, we can set it on the game mode via C++
	PlayerStateClass = ASPlayerState::StaticClass();

	SaveSlotName = TEXT("SaveSlot01");
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	GetWorldTimerManager().SetTimer(SpawnBotTimerHandle, this, &ASGameModeBase::SpawnBotTimerElapsed, BotSpawnInterval, true);

	UEnvQueryInstanceBlueprintWrapper* PowerupQuery
		= UEnvQueryManager::RunEQSQuery(this, GetPowerupSpawnLocationsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);

	if (ensure(PowerupQuery))
	{
		PowerupQuery->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupQueryFinished);
	}
}

void ASGameModeBase::OnActorKilled(AActor* Victim, AActor* Killer)
{
	/* If it's a player that has died */
	if (ASCharacter* SCharacter = Cast<ASCharacter>(Victim))
	{
		/* Local timer handle because if it was a member, then if two players died straight after
		 * one another, the second setting of this timer handle would cause the first to prematurely
		 * expire. Of course, it being local means we lose the reference to it, so we cannot stop
		 * this timer manually, we always have to wait for it to timeout. */
		FTimerHandle PlayerRespawnTimerHandle;
		FTimerDelegate RespawnPlayerDelegate;
		RespawnPlayerDelegate.BindUFunction(this, "RespawnPlayerElapsed", SCharacter->GetController());
		float RespawnTimerDuration = 2.0f; // TODO: expose as tweakable property
		GetWorldTimerManager().SetTimer(PlayerRespawnTimerHandle, RespawnPlayerDelegate, RespawnTimerDuration, false);
	}
	else if (ASAICharacter* AICharacter = Cast<ASAICharacter>(Victim))
	{
		/* Grant credits to the killer */
		APawn* PlayerPawn = Cast<APawn>(Killer);
		if (!PlayerPawn) return;

		APlayerController* PC = PlayerPawn->GetController<APlayerController>();
		if (!PC) return;
		ASPlayerState* PlayerState = PC->GetPlayerState<ASPlayerState>();
		if (!PlayerState) return;

		PlayerState->AddCredits(5);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s was killed by %s"), *GetNameSafe(Victim), *GetNameSafe(Killer));
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	ReadSaveGame();
}

void ASGameModeBase::WriteSaveGame()
{
	CurrentSaveGame->SavedActors.Empty();

	/* Single player save game functionality - does not work for multiplayer, would need to add an
	 * array of PlayerCredit structs into USSaveGame, which stores a player id and the number of
	 * credits for that player. For now, we just save whatever happens to be the first player. */
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		ASPlayerState* SPlayerState = Cast<ASPlayerState>(PlayerState);
		if (SPlayerState)
		{
			SPlayerState->SavePlayerState(CurrentSaveGame);
			break;
		}
	}

	/* Save all actors who implemented the gameplay interface */
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->Implements<USGameplayInterface>())
		{
			FActorSaveData ActorSaveData;
			ActorSaveData.Name = Actor->GetName();
			ActorSaveData.Transform = Actor->GetActorTransform();

			/* Serialize this actor's SaveData members to the ActorSaveData's buffer of bytes */

			// create a memory writer that will write to said buffer of bytes
			FMemoryWriter MemoryWriter(ActorSaveData.Bytes);
			FObjectAndNameAsStringProxyArchive ProxyArchive(MemoryWriter, true);
			// the archive is for a save game, meaning it will look for members marked up with SaveGame
			ProxyArchive.ArIsSaveGame = true;
			Actor->Serialize(ProxyArchive);

			CurrentSaveGame->SavedActors.Add(ActorSaveData);
		}
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, 0);
}

void ASGameModeBase::ReadSaveGame()
{
	/* If there is a pre-existing save game with this name, then get it */
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (CurrentSaveGame)
		{
			UE_LOG(LogTemp, Display, TEXT("Successfully read in save game, restoring data"));

			/* Get only those actors that are possible matches i.e. implement the interface */
			TArray<AActor*> GameplayInterfaceActors;
			UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USGameplayInterface::StaticClass(), GameplayInterfaceActors);

			/* Iterate over the saved actors, considering it is a subset of all actors */
			for (FActorSaveData& ActorSaveData : CurrentSaveGame->SavedActors)
			{
				/* Find the match */
				for (AActor* Actor : GameplayInterfaceActors)
				{
					if (Actor->GetName() == ActorSaveData.Name)
					{
						Actor->SetActorTransform(ActorSaveData.Transform);

						FMemoryReader MemoryReader(ActorSaveData.Bytes);
						FObjectAndNameAsStringProxyArchive ProxyArchive(MemoryReader, true);
						ProxyArchive.ArIsSaveGame = true;
						// with the reader, serialize will actually de-serialize the bytes back into
						// the actor's member variables
						Actor->Serialize(ProxyArchive);

						ISGameplayInterface::Execute_OnActorLoaded(Actor);

						break;
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to read in save game, not restoring data from save"));
		}
		return;
	}
	
	/* Otherwise create a new save game */
	CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
	const FString Msg = CurrentSaveGame ?
		TEXT("Successfully created new save game object") : TEXT("Failed to create new save game object");
	UE_LOG(LogTemp, Display, TEXT("%s"), *Msg);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	/* Need to call the implementation in the parent class. Calling the thunk will lead to 
	 * an infinite loop of calling thunk->this->thunk->this. */
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (ASPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ASPlayerState>())
	{
		NewPlayerState->LoadPlayerState(CurrentSaveGame);
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	/* If we have disabled spawning bots, skip spawning the bot! */
	if (!CVar_SpawnBots.GetValueOnGameThread()) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Spawning bots has been disabled."));
		return; 
	}

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
		/* Use the static helper function instead of doing a cast+getcomponentbyclass here manually */
		USAttributeComponent* AttributeComp
			= USAttributeComponent::GetAttributeComponent(CurrBot);

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

void ASGameModeBase::OnPowerupQueryFinished(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	/* If the query returns any result other than success, we consider it a failure. */
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get Powerups Spawn Locations query failed!"));
		return;
	}

	/* Query succeeded, get results. If there are none, return. */
	TArray<FVector> Locations;
	if (!QueryInstance->GetQueryResultsAsLocations(Locations)) { return; }

	ensure(PowerupClasses.Num() > 0);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 i = 0; i < NumPowerupsToSpawn; ++i)
	{
		const int32 RandomIndex = FMath::RandRange(0, Locations.Num() - 1);
		/* Spawn a random one of the powerup classes. Note that we cannot spawn ASCoin::StaticClass() or ASHealthPowerup::StaticClass()
		 * here because those have no assets assigned (no meshes etc.), and so would just be invisible in the world. This is why we
		 * created an editable TArray of ASPowerupBase's, so that we can assigned the blueprint child classes that have assets assigned. */
		GetWorld()->SpawnActor<AActor>(
			PowerupClasses[FMath::RandRange(0, PowerupClasses.Num() - 1)], Locations[RandomIndex], FRotator::ZeroRotator, SpawnParameters
		);
		Locations.RemoveAt(RandomIndex);
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		/* If the player controller already has a Pawn then it will re-use it. We don't want to
		 * do this, so unpossess the current pawn so we get a brand new one. */
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void ASGameModeBase::KillAI()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* CurrBot = *It;
		/* Use the static helper function instead of doing a cast+getcomponentbyclass here manually */
		USAttributeComponent* AttributeComp
			= USAttributeComponent::GetAttributeComponent(CurrBot);

		/* Ignoring the result of Kill, don't care if it fails because that means they are already dead. */
		AttributeComp->Kill(this); // TODO: maybe pass in player controller as instigator to get credits?
	}
}
