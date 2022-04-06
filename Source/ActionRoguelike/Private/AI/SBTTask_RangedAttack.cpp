// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


/* Spawn a projectile aimed towards the target actor. */
EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	/* Get the blackboard from the behavior tree */
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!ensure(Blackboard)) return EBTNodeResult::Failed;

	/* Get the target actor key from the blackboard */
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
	if (!ensure(TargetActor)) return EBTNodeResult::Failed;

	/* Get the pawn who owns this blackboard/behavior tree, via the AIController */
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!ensure(AIController)) return EBTNodeResult::Failed;
	/* Cast to character because we want the socket location for spawning */
	ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());

	/* Get the muzzle socket so we can spawn the projectile there.
	 * Would be better to not hard code this... */
	const FVector MuzzleLocation = AICharacter->GetMesh()->GetSocketLocation("Muzzle_01");
	const FVector TargetActorLocation = TargetActor->GetActorLocation();

	/* Destination - Start to get direction vector, then take rotation of it */
	const FRotator MuzzleRotation = (TargetActorLocation - MuzzleLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = AICharacter;
	return GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams)
		? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
