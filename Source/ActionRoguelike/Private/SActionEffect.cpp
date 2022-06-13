// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "SActionComponent.h"


USActionEffect::USActionEffect()
{
	/* Action effects will pretty much always be auto start effects */
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		/* Set a one-shot timer that stops this action */
		FTimerDelegate Delegate; // Gets copied so it doesn't matter that it's a local var and will go out of scope
		Delegate.BindUFunction(this, "StopAction", Instigator);
		GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		/* Set a looping timer that expires every Period and calls the execute periodic effect function */
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);
		GetWorld()->GetTimerManager().SetTimer(PeriodTimerHandle, Delegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	/* We do this because if we set a duration of 3 seconds, and a period of 1 second, we would
	 * expect to get 3 ticks. But if the duration timer triggers before the period timer, then
	 * because it cancels both below, we won't get the last period tick. For this reason, we
	 * check whether the period timer is about to elapse, and if so, manually call the function. 
	 * We must do this before calling Super::StopAction in case that alters the state and we rely
	 * on said state in our ExecutePeriodicEffect implementation. */
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodTimerHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);

	USActionComponent* OwningActionComp = GetOwningComponent();
	if (OwningActionComp)
	{
		OwningActionComp->RemoveAction(this);
	}
}

float USActionEffect::GetTimeRemaining() const
{
	const float EndTime = TimeStarted + Duration;
	const float TimeRemaining = EndTime - GetWorld()->TimeSeconds;
	return TimeRemaining >= 0 ? TimeRemaining : 0.0f;
}

void USActionEffect::ExecutePeriodicEffect_Implementation(AActor* InstigatorActor)
{

}
