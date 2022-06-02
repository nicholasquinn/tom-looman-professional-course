// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"
#include "../ActionRoguelike.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		/* Start the default actions, owner is instigator for all of them */
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}

}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString Msg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, Msg);

	for (USAction* Action : Actions)
	{
		FColor TextColor = Action->GetIsRunning() ? FColor::Blue : FColor::White;
		FString ActionLogString = FString::Printf(TEXT("[%s] Action = %s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Action)
		);
		// Tick log, so duration must be 0.0f so as not to spam the screen
		MultiplayerScreenLog(this, ActionLogString, TextColor, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* InstigatorActor, TSubclassOf<USAction> ActionClass)
{
	/* The reason the ! is outside the ensure is because we want to ensure the condition is true (i.e. our
	 * assumptions are that we have an ActionClass, InstigatorActor, and that InstigatorActor has 
	 * authority), but we want to exit when the condition is false i.e. when we don't have an ActionClass */
	if (!ensure(ActionClass)) { return; }
	if (!ensure(InstigatorActor)) { return; }
	if (!ensure(InstigatorActor->HasAuthority())) { return; }

	/* Custom function for creating USAction UObjects as they need some further initialization. */
	USAction* ActionInstance = USAction::New(GetOwner(), this, ActionClass);

	/* If we already have this action, don't add it. */
	if (HasAction(ActionInstance->ActionName)) { return; }

	if (ensure(ActionInstance))
	{
		Actions.Add(ActionInstance);
		/* we assume an autostart action can always start... otherwise it doesn't make sense! */
		if (ActionInstance->bAutoStart && ensure(ActionInstance->CanStart(InstigatorActor)))
		{
			ActionInstance->StartAction(InstigatorActor);
		}
	}
}

void USActionComponent::RemoveAction(USAction* Action)
{
	// if action is null, early exit. If it isn't null, but is still running, also early exit.
	if (!ensure(Action && !Action->GetIsRunning())) { return; }
	Actions.Remove(Action);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			/* We found an action with the given name, but can we start it? */
			if (!Action->CanStart(Instigator))
			{
				/* we cannot, but continue looking as there might be multiple actions with the same name */
				GEngine->AddOnScreenDebugMessage(
					-1, 2.0f, FColor::Red, FString::Printf(
						TEXT("Failed to start action: "), *(ActionName.ToString())
					)
				);
				continue;
			}

			/* Start the action locally (whether we are client or server). This will set the bIsRunning replicated
			 * variable to true. Note that if we are the server, then because it's an onrep variable, it will call
			 * the onrep function on all clients. If we are the client, this doesn't happen, instead see below... */
			Action->StartAction(Instigator);

			/* if we are a Client */
			if (!GetOwner()->HasAuthority())
			{
				/* Tell the server to run this as well, which in turn will cause the onrep function for bIsRunning
				 * to be called on all clients, including US. However, we already set bIsRunning locally above when
				 * we called StartAction. We will still get the event from the server saying bIsRunning was changed,
				 * but because we already have the same value because we set it locally, we won't run the onrep. 
				 * Onrep functions only run when the variables value is different, not on every change. */
				ServerStartAction(Instigator, ActionName);
			}

			return true;
		}
	}
	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName && Action->GetIsRunning())
		{
			if (!Instigator->HasAuthority())
			{
				ServerStopAction(Instigator, ActionName);
			}
			Action->StopAction(Instigator);
			return true;
		}
	}
	return false;
}

bool USActionComponent::HasAction(FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action->ActionName == ActionName) { return true; }
	}
	return false;
}

/* Required in order for UActorComponents to be able to replicate sub-objects. This implementation is very similar to the 
 * implementation in AActor.cpp */
bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	bool bWroteSomething = false;
	for (USAction* Action : Actions)
	{
		/* Replicate the sub-object via the same channel as this actor/component. Re-watch A.W.Forsythe's video about Unreal Networking
		 * for a recap on actor channels. TLDR: there is one channel per actor in your game. That actor, as well as all of its components,
		 * is replicated via that channel. Check out the implementation of replicate sub-objects in AActor.h, you will see it stores an
		 * array of all its replicated components, and it basically does the same thing as this here, but with that array of components,
		 * instead of an array of UObjects. In fact, for each component, it calls ActorComp->ReplicateSubobjects(Channel, Bunch, RepFlags);,
		 * i.e. THIS FUNCTION! */
		bWroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
	}
	return bWroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* The usual rep lifetime is still used for TArrays of UObjects, there is just extra boilerplate
	 * in the UObject itself. */
	DOREPLIFETIME(USActionComponent, Actions);
}

