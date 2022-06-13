// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include <SActionComponent.h>
#include "../ActionRoguelike.h"
#include "Net/UnrealNetwork.h"


void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is starting action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
	//const FString LogText = FString::Printf(TEXT("%s is starting action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
	//MultiplayerScreenLog(this, LogText, FColor::Orange);

	//ensureAlways(!bIsRunning); // If we're trying to start something that's already running, something's gone wrong

	/* Now that we're starting this action, add all of its Grants tags to the owning action component. */
	USActionComponent* OwningActionComp = GetOwningComponent();
	ensure(OwningActionComp);
	OwningActionComp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
	// did I think this was a ctor? why did it set bAutoStart here?
	bAutoStart = false; // The majority of actions are started by input, not automatically 

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);

	TimeStarted = GetWorld()->TimeSeconds;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is stopping action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
	//const FString LogText = FString::Printf(TEXT("%s is stopping action %s"), *GetNameSafe(Instigator), *ActionName.ToString());
	//MultiplayerScreenLog(this, LogText, FColor::Orange);

	//ensureAlways(bIsRunning); // if we are trying to stop something that isn't running, something has gone wrong

	/* Now that we're stopping this action, remove all of its Grants tags from the owning action component. */
	USActionComponent* OwningActionComp = GetOwningComponent();
	ensure(OwningActionComp);
	OwningActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

USAction* USAction::New(
	AActor* OwningActor, USActionComponent* OwningActionComponent, TSubclassOf<USAction> ActionClass
)
{
	/* Owner of UObject must be AActor type when it's a networked UObject, this is just an engine
	 * mandated thing, but the annoying part is it won't error at compile time, it will simply
	 * change the owner to the Actor who owns this component if you try set it to the component
	 * e.g. this. This will lead to a crash if you then try do something like GetOwner and cast
	 * to UActionComponent for example. */
	USAction* NewAction = NewObject<USAction>(OwningActor, ActionClass);
	if (NewAction)
	{
		NewAction->OwningActionComponent = OwningActionComponent;
	}
	return NewAction;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return OwningActionComponent;
	//return Cast<USActionComponent>(GetOuter());
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	return !GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags) && !RepData.bIsRunning;
}

/* An Actor is something that is placed in the world. UObjects however are not themselves
 * placed in the world, and so while GetWorld exists as a function, it's not actually 
 * implemented. Use the Outer's GetWorld. */
UWorld* USAction::GetWorld() const
{
	/* Due to the engine mandating that the outer for this UObject be the player character class
	 * that owns the component that owns this UObject, we must return the outer as AActor. Even if
	 * you set the outer to be the owning component on creation, unreal engine will forcefully set
	 * it back to the player character class anyway, as it's mandated by the engine. */
	AActor* OwningActor = Cast<AActor>(GetOuter());
	/* The outer can still be null though, because the editor itself will create instances of this 
	 * UObject (I assume for things like CDO), and we don't really know what it will pass for the
	 * outer when it does so. Even if it weren't possible, it's still always better to if guard. */
	return OwningActor ? OwningActor->GetWorld() : nullptr;
}

bool USAction::GetIsRunning() const
{
	return RepData.bIsRunning;
}

void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, OwningActionComponent);
}

