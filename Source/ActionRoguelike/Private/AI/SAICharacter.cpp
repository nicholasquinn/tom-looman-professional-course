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
#include "SWorldUserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "SActionComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

	/* Temporary fix to let magic projectile hit the mesh rather than the capsule component.
	 * The projectile should really have its own object type, rather than using world dynamic,
	 * that's too general. */
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	/* Will be possessed by an AI controller regardless of whether it was pre-placed into the
	 * level or spawned at runtime. Of course, if this character is possessed by a Player Controller
	 * than this is irrelevant. */
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HitTimeName = "HitTime";
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
		GetMesh()->SetScalarParameterValueOnMaterials(HitTimeName, GetWorld()->TimeSeconds);
	}

	/* If our health was changed in any way */
	if (Delta != 0.0f)
	{
		/* Create the health bar widget only on the first time health changes */
		if (!HealthBarWidget)
		{
			/* Create widget can only be used to create UserWidget types, and the OwningObject (first param) must be
			 * one of the following types: UWidgetTree, APlayerController, UGameInstance, or UWorld. UWorld is usually
			 * the easiest to pass in. */
			HealthBarWidget = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (HealthBarWidget)
			{
				/* Since adding the widget to the viewport will cause its OnConstruct event to run, we must
				 * set the OwningActor property before adding to viewport, since we intend to use it in
				 * OnConstruct. */
				HealthBarWidget->OwningActor = this;
				HealthBarWidget->AddToViewport();
			}
		}
	}

	/* --- ON DEATH --- */

	/* This is the actual/real delta that is sent to us in the delegate, so it will be strictly less than 0 if we really have
	 * taken damage. So, if we have taken damage and our health is < 0, then this is the instance of the event that we have
	 * died on (subsequent hits will have Delta == 0). Note that it's probably a better design to have this logic in the 
	 * attribute component and have a second event, OnDeath, that does the below logic and raises the event in that case. */
	if (Delta < 0.0f && NewHealth <= 0.0f)
	{
		/* Remove health bar from the viewport. This could have also been done in the blueprint derived class (WorldWBP_AiHealthBar)
		 * in the OnHealthChanged event blueprint graph, but then other types of UWorldUserWidget would also be responsible for doing
		 * the same thing. It's assumed all types of widget will want to be deleted once the AI is dead, so we do both construction 
		 * (see above) and deletion (see below) in C++, and just let the blueprint subclasses implement the custom behavior in their
		 * BP Graphs. */
		if (HealthBarWidget)
		{
			HealthBarWidget->RemoveFromParent();
		}

		// We just died so stop AI logic if it's present
		AAIController* AIC = Cast<AAIController>(GetController());
		/* We don't ensure this AIC because this code will run on clients and they do not have AI Controllers as they
		 * exist only on the server. Clients only have their player controller. This ensure would therefore always 
		 * trigger. We could check if (HasAuthority() && ensure(AIC)) if we wanted to though. */
		if (AIC)
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

		/* Also disable collision, otherwise capsule component is left standing in the world and can block player,
		 * camera, or projectiles etc. */
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		/* And now that we have no collision enabled, the character movement component will think we are in the air,
		 * and thus try to apply gravity. To prevent this, disable movement from it. */
		GetCharacterMovement()->DisableMovement();

		/* Destroy 10 seconds from now, gives us a chance to do the ragdoll. If we were to call Destroy() here instead,
		 * then the bot would disappear instantly as soon as it reaches 0HP, and we wouldn't see the rag doll happpen.
		 * SetLifeSpan simply sets a timer for the given duration and then calls Destroy. */
		SetLifeSpan(10);
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = GetController<AAIController>();
	if (AIController)
	{
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		if (ensureMsgf(Blackboard, TEXT("Blackboard asset not set for ASAICharacter.")))
		{
			/* Skip if the new target is the same as the existing target */
			AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
			if (CurrentTarget == NewTarget) { return; }

			/* We have a new target, so play the alert */
			ShowAlert();

			/* This string should probably be an FNAME member exposed via UPROPERTY at least,
			 * and better yet would be to use a BlackboardKeySelector, and then access
			 * the key name field, but this hard-coded way will suffice for now. */
			Blackboard->SetValueAsObject("TargetActor", NewTarget);
		}
	}
}

void ASAICharacter::ShowAlert_Implementation()
{
	ensure(AlertWidgetClass);

	/* Lazily create an instance of the alert widget and add it to the viewport */
	if (!AlertWidget)
	{
		AlertWidget = Cast<USWorldUserWidget>(CreateWidget(GetWorld(), AlertWidgetClass));
		if (!ensure(AlertWidget)) { return; }
		AlertWidget->OwningActor = this;
		AlertWidget->AddToViewport();
	}

	/* Show the widget */
	AlertWidget->SetVisibility(ESlateVisibility::Visible);

	/* Set timer to hide the widget */
	FTimerHandle Unused;
	GetWorldTimerManager().SetTimer(Unused, this, &ASAICharacter::HideAlert, 1.0f);
}

void ASAICharacter::HideAlert()
{
	AlertWidget->SetVisibility(ESlateVisibility::Hidden);
}
