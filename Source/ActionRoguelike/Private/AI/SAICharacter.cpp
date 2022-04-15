// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <DrawDebugHelpers.h>
#include <Perception/PawnSensingComponent.h>


// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	/* Will be possessed by an AI controller regardless of whether it was pre-placed into the
	 * level or spawned at runtime. Of course, if this character is possessed by a Player Controller
	 * than this is irrelevant. */
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

/* When we see a Pawn (via the pawn sensing component), we set the blackboard key
 * for the TargetActor to be this seen Pawn. */
void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (ensureMsgf(Blackboard, TEXT("Blackboard asset not set for ASAICharacter.")))
		{
			/* This should probably be an FNAME member exposed via UPROPERTY at least,
			 * and better yet would be to use a BlackboardKeySelector, and then access
			 * the key name field, but this hard-coded way will suffice for now. */
			Blackboard->SetValueAsObject("TargetActor", Pawn);
		}
	}

	DrawDebugString(GetWorld(), Pawn->GetActorLocation(), TEXT("Pawn Spotted!"), nullptr, FColor::Red, 1.0f, true);
}
