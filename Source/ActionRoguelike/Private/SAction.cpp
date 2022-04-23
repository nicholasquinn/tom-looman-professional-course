// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include <SActionComponent.h>


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is starting action %s"), *GetNameSafe(Instigator), *ActionName.ToString());

	ensureAlways(!bIsRunning); // If we're trying to start something that's already running, something's gone wrong

	/* Now that we're starting this action, add all of its Grants tags to the owning action component. */
	USActionComponent* OwningActionComp = GetOwningComponent();
	ensure(OwningActionComp);
	OwningActionComp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is stopping action %s"), *GetNameSafe(Instigator), *ActionName.ToString());

	ensureAlways(bIsRunning); // if we are trying to stop something that isn't running, something has gone wrong

	/* Now that we're stopping this action, remove all of its Grants tags from the owning action component. */
	USActionComponent* OwningActionComp = GetOwningComponent();
	ensure(OwningActionComp);
	OwningActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

USActionComponent* USAction::GetOwningComponent()
{
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	return !GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags) && !bIsRunning;
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

bool USAction::GetIsRunning() const
{
	return bIsRunning;
}

