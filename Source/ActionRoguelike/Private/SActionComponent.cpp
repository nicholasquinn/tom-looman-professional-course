// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		AddAction(ActionClass);
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString Msg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, Msg);
}

void USActionComponent::AddAction(TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass)) { return; }

	USAction* ActionInstance = NewObject<USAction>(this, ActionClass);
	if (ensure(ActionInstance))
	{
		Actions.Add(ActionInstance);
	}
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

