// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SSaveGame.h"

ASPlayerState::ASPlayerState()
{
	Credits = StartingCredits;
}

void ASPlayerState::AddCredits(int32 NumCredits)
{
	ensure(NumCredits > 0);
	Credits += NumCredits;
	OnCreditsChanged.Broadcast(this, NumCredits, Credits);
}

bool ASPlayerState::RemoveCredits(int32 NumCredits)
{
	ensure(NumCredits > 0);
	if (Credits >= NumCredits)
	{
		Credits -= NumCredits;
		OnCreditsChanged.Broadcast(this, NumCredits, Credits);
		return true;
	}
	return false;
}

int32 ASPlayerState::GetCredits()
{
	return Credits;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveGame)
{	
	if (SaveGame) { SaveGame->Credits = Credits; }
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveGame)
{
	if (SaveGame) { Credits = SaveGame->Credits; }
}
