// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SSaveGame.h"
#include <Net/UnrealNetwork.h>

ASPlayerState::ASPlayerState()
{
	Credits = StartingCredits;
	bReplicates = true;
}

void ASPlayerState::AddCredits(int32 NumCredits)
{
	ensure(NumCredits > 0);
	Credits += NumCredits;
	OnCreditsChanged.Broadcast(this, NumCredits, Credits);
	//MulticastOnCreditsChanged(NumCredits);
}

bool ASPlayerState::RemoveCredits(int32 NumCredits)
{
	ensure(NumCredits > 0);
	if (Credits >= NumCredits)
	{
		Credits -= NumCredits;
		OnCreditsChanged.Broadcast(this, NumCredits, Credits);
		//MulticastOnCreditsChanged(NumCredits);
		return true;
	}
	return false;
}

int32 ASPlayerState::GetCredits()
{
	return Credits;
}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits - OldCredits, Credits);
}

/* // Switched to OnRep_Credits version
void ASPlayerState::MulticastOnCreditsChanged_Implementation(int32 NumCredits)
{
	OnCreditsChanged.Broadcast(this, NumCredits, Credits);
}
*/

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveGame)
{	
	if (SaveGame) { SaveGame->Credits = Credits; }
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveGame)
{
	if (SaveGame) { Credits = SaveGame->Credits; }
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}