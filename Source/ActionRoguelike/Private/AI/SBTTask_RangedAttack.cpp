// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "SAttributeComponent.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpreadDegrees = 5.0f;
}

/* Spawn a projectile aimed towards the target actor. */
EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	/* Get the blackboard from the behavior tree */
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!ensure(Blackboard)) return EBTNodeResult::Failed;

	/* Get the target actor key from the blackboard */
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
	if (!ensure(TargetActor)) return EBTNodeResult::Failed;

	/* If the target actor is not alive, then we don't want to target them anymore.
	 * Note that we do not, and should not, set the target actor blackboard key to nullptr.
	 * That is not our responsibility as a BB Task. We just say what the result of the 
	 * task is e.g. success, fail etc.*/
	if (!USAttributeComponent::IsActorAlive(TargetActor)) { return EBTNodeResult::Failed; }

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
	FRotator MuzzleRotation = (TargetActorLocation - MuzzleLocation).Rotation();
	const FVector BarrelOffset = MuzzleRotation.Vector().GetSafeNormal() * 100;

	/* Add some random deviation from perfect aim, so it's possible for the bots to miss. Note that
	 * roll doesn't need to be adjust, as that will just roll the projectile which has no effect. Also
	 * note that we don't allow the bot to shoot downwards of the player, only directly at the same height
	 * or above. This just seems a little more realistic. */
	MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpreadDegrees);
	MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpreadDegrees, MaxBulletSpreadDegrees);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = AICharacter;
	
	return GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation + BarrelOffset, MuzzleRotation, SpawnParams)
		? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
