// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckLowHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

void USBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	/* We assume that the SelfActor key is an actor which has an attribute component.
	 * We will add the idea of 'low health' to the attribute component, as it's
	 * its responsibility to be determining health based things like that. */

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	/* It is expected that we have a blackboard assigned to our behavior tree... */
	if (!ensure(Blackboard)) { return; }

	AActor* SelfActor = Cast<AActor>(Blackboard->GetValueAsObject(SelfKeySelector.SelectedKeyName));
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(SelfActor);
	/* It is expected that we would only be using this service when the self actor does have an
	 * an AttributeComponent, so we want to be made aware if it does not have one. */
	if (!ensure(AttributeComp)) { return; }

	/* Set the blackboard key for isLowHealth according to the return value of the attribute comp. */
	Blackboard->SetValueAsBool(IsLowHealthKeySelector.SelectedKeyName, AttributeComp->IsLowHealth());
}
