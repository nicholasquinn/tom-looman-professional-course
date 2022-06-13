// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "SActionComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	ActionComponent = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComponent"));

	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	/* The below 3 settings give the effect of being able to look around without the player itself rotating when standing still,
	 * but then rotating the player over time to face the movement of direction once moving. */

	/* 1. We want to orient the spring arm to the same rotation as the player controller, that way we can look around with the camera. */
	SpringArmComp->bUsePawnControlRotation = true;

	/* 2. Don't make the Character's Yaw rotation match the Controller's yaw rotation, this is what allows us to look around
	 * freely when not moving i.e. you can rotate the camera around to look at the character's face. */
	bUseControllerRotationYaw = false;
	
	/* 3. bOrientRotationToMovement rotates the Character toward the direction of acceleration, using RotationRate as the rate of rotation change.
	 * This is what makes the character correct its rotation once moving e.g. if we are looking at the character face, then move left, the
	 * Character will rotate to be facing the left. Normally you will want to make sure that other settings are cleared, such as 
	 * bUseControllerRotationYaw on the Character, as they go against this behavior, as explained above.
	 * Note: there is also bUseControllerDesiredRotation, which smoothly rotates the Character toward the Controller's desired rotation
	 * i.e. where you look (Controller->ControlRotation), the character lerps to look there also, using RotationRate as the rate of rotation change. 
	 * OrientRotationToMovement takes precedence over bUseControllerDesiredRotation. */
	GetCharacterMovement()->bOrientRotationToMovement = true;

	/* Set constants */
	HitTimeName = "HitTime";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	/* Listen to events raised from our own attribute component. Considering we only
	 * actually do anything if the health change event resulted in death, it may be 
	 * worth creating an OnDeath event that the attribute component only raises when
	 * the health falls below 0. */
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}


void ASCharacter::MoveX(float AxisValue)
{
	FRotator ControllerRotation = GetControlRotation();
	/* Zero out Pitch and Roll, only interested in Yaw as it is rotation around the Z (up)
	 * Axis, and therefore deals with movement direction. */
	ControllerRotation.Pitch = 0;
	ControllerRotation.Roll = 0;

	/* No need to clamp AxisValue for ACharacter, it is done internally - see CharacterMovementComponent.cpp 7094 */
	AddMovementInput(ControllerRotation.Vector(), AxisValue);
}


void ASCharacter::MoveY(float AxisValue)
{
	FRotator ControllerRotation = GetControlRotation();
	/* Zero out Pitch and Roll, only interested in Yaw as it is rotation around the Z (up)
	 * Axis, and therefore deals with movement direction. */
	ControllerRotation.Pitch = 0; 
	ControllerRotation.Roll = 0;

	/* Basically stealing the implementation of this Blueprint function from KismetMathLibrary 815
	 * We could (and probably should) include KismetMathLibrary rather than stealing.
	 * Note that we don't bother copying FVector UKismetMathLibrary::GetForwardVector(FRotator InRot)
	 * above as it is literally return InRot.Vector();
	 * 
	 *	FVector UKismetMathLibrary::GetRightVector(FRotator InRot)
	 *	{
	 *		return FRotationMatrix(InRot).GetScaledAxis(EAxis::Y);
	 *	}
	 *
	*/
	const FVector ControllerRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);

	AddMovementInput(ControllerRightVector, AxisValue);
}


void ASCharacter::StartSprinting()
{
	ActionComponent->StartActionByName(this, "Sprint");
}


void ASCharacter::StopSprinting()
{
	ActionComponent->StopActionByName(this, "Sprint");
}


void ASCharacter::OnHealthChanged(AActor* InstigatorActor, class USAttributeComponent* OwningComponent, float NewHealth, float Delta)
{
	/* If we've been damaged (rather than healed) */
	if (Delta < 0.0f)
	{
		/* It seems like both mesh types, static and skeletal, have the ability to set scalar parameter values
		 * on their assigned materials. */
		GetMesh()->SetScalarParameterValueOnMaterials(HitTimeName, GetWorld()->TimeSeconds);

		/* and we've died as a result of it */
		if (!AttributeComp->IsAlive())
		{
			AttributeComp->ClearEvents();
			DisableInput(Cast<APlayerController>(GetController()));
			SetLifeSpan(5.0f);
		}
	}
}


void ASCharacter::PrimaryAttack()
{
	ActionComponent->StartActionByName(this, "BasicProjectile");
}


void ASCharacter::SecondaryAttack()
{
	ActionComponent->StartActionByName(this, "DashProjectile");
}


void ASCharacter::UltimateAttack()
{
	ActionComponent->StartActionByName(this, "BlackHoleProjectile");
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawControlVsPawnRotationDebugArrows();
}


// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &ASCharacter::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ASCharacter::MoveY);

	PlayerInputComponent->BindAxis("LookX", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookY", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("UltimateAttack", IE_Pressed, this, &ASCharacter::UltimateAttack);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	/* You can bind directly to the instance of the interaction component that this character class owns. 
	 * You don't need to make a middle-man method. */
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this->InteractionComp, &USInteractionComponent::PrimaryInteract);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprinting);
}


FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}


void ASCharacter::DrawControlVsPawnRotationDebugArrows()
{
	/* --Rotation Visualization-- */
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}


void ASCharacter::AddHealth(float Amount /*= 100*/)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

