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

/* An Actor is something that is placed in the world. UObjects however are not themselves
 * placed in the world, and so while GetWorld exists as a function, it's not actually 
 * implemented. Use the Outer's GetWorld. */
UWorld* USAction::GetWorld() const
{
	/* We set the outer to the SActionComponent that "owns" this action when it created it
	 * with the call to NewObject. SActionComponent is an ActorComponent which has GetWorld
	 * implemented. */
	UActorComponent* ActorComp = Cast<UActorComponent>(GetOuter());
	/* The outer can still be null though, because the editor itself will create instances of this 
	 * UObject (I assume for things like CDO), and we don't really know what it will pass for the
	 * outer when it does so. */
	return ActorComp ? ActorComp->GetWorld() : nullptr;
}

