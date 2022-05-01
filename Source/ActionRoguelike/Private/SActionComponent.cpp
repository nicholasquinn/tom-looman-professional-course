// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Start the default actions, owner is insigator for all of them */
	for (TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		AddAction(GetOwner(), ActionClass);
	}
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString Msg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, Msg);
}

void USActionComponent::AddAction(AActor* InstigatorActor, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass)) { return; }

	USAction* ActionInstance = NewObject<USAction>(this, ActionClass);

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

			Action->StartAction(Instigator);

			/* if we are a Client */
			if (!GetOwner()->HasAuthority())
			{
				/* Tell the server to run this as well */
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

