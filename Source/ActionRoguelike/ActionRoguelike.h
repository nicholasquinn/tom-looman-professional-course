// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/* Prints the given message to the screen and prefixes with Server or Client depending
 * on where this code is running. This is like a static library function and is available from everywhere.
 * You're better of making libraries when you have multiple functions that can be logically grouped, but
 * for one-off helper functions, it's not the end of the world to put them here. */
static void MultiplayerScreenLog(UObject* WorldContextObject, FString Message, FColor Color = FColor::Black, float Duration = 5.0f)
{
	if (!ensure(GEngine)) { return; }

	if (!ensure(WorldContextObject)) { return; }

	UWorld* World = WorldContextObject->GetWorld();
	if (!ensure(World)) { return; }

	const FString MultPrefix = World->GetNetMode() == NM_Client ? "[Client] " : "[Server] ";
	GEngine->AddOnScreenDebugMessage(-1, Duration, Color, MultPrefix + Message);
}
