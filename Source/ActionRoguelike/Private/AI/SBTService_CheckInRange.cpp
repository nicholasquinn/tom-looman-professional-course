// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckInRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


USBTService_CheckInRange::USBTService_CheckInRange()
{
	Range = 2000.0f; // 20m range default
}

void USBTService_CheckInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	/* OwnerComp is a BehaviorTree, which is what has a Blackboard set on it */
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		/* Target actor may be null (never set, gets unset, target actor gets destroyed etc.) */
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (!TargetActor) return;

		/* The behavior tree itself is owned by an AIController. This is not to be confused
		 * with the general concept of an owning actor... i.e. GetOwner(). This should be
		 * set, hence the ensure. */
		AAIController* AIController = OwnerComp.GetAIOwner();
		if (!ensure(AIController)) return;

		/* Use the AI Controller to get the pawn it is controlling i.e. the AI Pawn. This
		 * should also be set, hence the ensure. */
		APawn* AIPawn = AIController->GetPawn();
		if (!ensure(AIPawn)) return;

		/* Now we have the target actor and the AI, get the distance between them */
		const float DistanceBetween = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
		const bool bInRange = DistanceBetween < Range;
		/* This requires TargetActor can be detected on the Visibility Trace Channel, see implementation for why...
		 * TLDR: it does a line trace using that channel to detect if it has L.O.S */
		const bool bHasLineOfSight = AIController->LineOfSightTo(TargetActor);

		/* Update the key specified by our selector */
		BlackboardComp->SetValueAsBool(InRangeKeySelector.SelectedKeyName, bInRange && bHasLineOfSight);
	}

}
