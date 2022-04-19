// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is starting action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is stopping action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
}

