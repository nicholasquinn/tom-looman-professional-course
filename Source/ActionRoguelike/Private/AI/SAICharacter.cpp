// Fill out your copyright notice in the Description page of Project Settings.

// this file
#include "AI/SAICharacter.h"

// my files
#include "SAttributeComponent.h"

// engine files
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <BrainComponent.h>
#include <DrawDebugHelpers.h>
#include <Perception/PawnSensingComponent.h>


// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	/* Will be possessed by an AI controller regardless of whether it was pre-placed into the
	 * level or spawned at runtime. Of course, if this character is possessed by a Player Controller
	 * than this is irrelevant. */
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

/* When we see a Pawn (via the pawn sensing component), we set the blackboard key
 * for the TargetActor to be this seen Pawn. */
void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);
	DrawDebugString(GetWorld(), Pawn->GetActorLocation(), TEXT("Pawn Spotted!"), nullptr, FColor::Red, 1.0f, true);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta)
{
	/* If our health was changed by anything that isn't us, and it was a damaging event */
	if (InstigatorActor != this && Delta < 0.0f)
	{
		SetTargetActor(InstigatorActor);
	}

	/* --- ON DEATH --- */

	/* This is the actual/real delta that is sent to us in the delegate, so it will be strictly less than 0 if we really have
	 * taken damage. So, if we have taken damage and our health is < 0, then this is the instance of the event that we have
	 * died on (subsequent hits will have Delta == 0). Note that it's probably a better design to have this logic in the 
	 * attribute component and have a second event, OnDeath, that does the below logic and raises the event in that case. */
	if (Delta < 0.0f && NewHealth <= 0.0f)
	{
		// We just died so stop AI logic if it's present
		AAIController* AIC = Cast<AAIController>(GetController());
		if (ensure(AIC))
		{
			/* If we're possessed by an AI (should be, hence the ensure). Note that this BrainComponent is just a super
			 * class of the UBehaviorTreeComponent, and we could just get a reference to the BT component instead, but
			 * the brain component actually owns the methods for starting, pausing, stopping AI 'thinking', so its more
			 * appropriate to call it from there, even though BT is the only implementation of this Brain component in unreal. */ 
			UBrainComponent* BrainComp = AIC->GetBrainComponent();
			/* Stop the brain from processing AI logic (the behavior tree in this case, although it could be a custom 
			 * implementation. The reason we are stopping logic processing is because we have just died. */
			BrainComp->StopLogic(TEXT("Killed"));
		}

		/* Regardless of being controlled or not, we want to rag doll. First we must set our profile to Ragdoll, as this
		 * has the correct settings for ragdolling (most importantly it has query+physics so it can physically simulate),
		 * and we actually have to tell the skeletal mesh to use physics rather than an animation. */ 
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetAllBodiesSimulatePhysics(true);

		/* Destroy 10 seconds from now, gives us a chance to do the ragdoll. If we were to call Destroy() here instead,
		 * then the bot would disappear instantly as soon as it reaches 0HP, and we wouldn't see the rag doll happpen.
		 * SetLifeSpan simply sets a timer for the given duration and then calls Destroy. */
		SetLifeSpan(10);
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (ensureMsgf(Blackboard, TEXT("Blackboard asset not set for ASAICharacter.")))
		{
			/* This string should probably be an FNAME member exposed via UPROPERTY at least,
			 * and better yet would be to use a BlackboardKeySelector, and then access
			 * the key name field, but this hard-coded way will suffice for now. */
			Blackboard->SetValueAsObject("TargetActor", NewTarget);
		}
	}
}
