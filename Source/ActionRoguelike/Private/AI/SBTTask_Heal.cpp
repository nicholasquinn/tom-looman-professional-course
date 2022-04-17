// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_Heal.h"

#include "SAttributeComponent.h"

#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	/* Same thing as the check low health service. Get the self actor and its attribute component,
	 * so that we can heal it. */
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	/* It is expected that we have a blackboard assigned to our behavior tree... */
	if (!ensure(Blackboard)) { return EBTNodeResult::Failed; }

	AActor* SelfActor = Cast<AActor>(Blackboard->GetValueAsObject(SelfKeySelector.SelectedKeyName));
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComponent(SelfActor);
	/* It is expected that we would only be using this task when the self actor does have an
	 * an AttributeComponent, so we want to be made aware if it does not have one. */
	if (!ensure(AttributeComp)) { return EBTNodeResult::Failed; }

	/* Heal ourselves (self actor) via the attribute component. We do this small calculation to heal only what is missing,
	 * rather than blatantly just saying MaxHealth - although might change to doing that, this is just a design decision. */
	bool bHealed = AttributeComp->ApplyHealthChange(SelfActor, AttributeComp->GetMaxHealth() - AttributeComp->GetHealth());

	/* Currently we consider the heal task a failure if we are already at max health. Again, just a design decision that
	 * can easily be changed. */
	return (bHealed) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
