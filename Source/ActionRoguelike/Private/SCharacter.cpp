// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

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

	/* Set a sensible default for the attack timer duration */
	PrimaryAttackTimerDuration = 0.2f;

	/* Normalized aim direction vector gets multiplied by this scaler */
	AimTraceDistance = 10000;

	/* Set constants */
	MuzzleName = "Muzzle_01";
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
			DisableInput(Cast<APlayerController>(GetController()));
		}
	}
}


void ASCharacter::PlayAttackEffects(UAnimMontage* AttackAnim)
{
	PlayAnimMontage(AttackAnim);
	UGameplayStatics::SpawnEmitterAttached(
		CastEffect, GetMesh(), MuzzleName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
	);
}

void ASCharacter::Attack(TSubclassOf<ASMagicProjectileBase> ProjectileTypeToSpawn)
{
	/* Perform a trace to try find the aim point. If nothing is hit, then just take
	 * the end of the trace as the aim point. */
	FHitResult OutHit;
	const FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComp->GetForwardVector() * AimTraceDistance);
	FCollisionObjectQueryParams AimTraceQueryObjectSettings;
	AimTraceQueryObjectSettings.AddObjectTypesToQuery(ECC_WorldStatic);
	AimTraceQueryObjectSettings.AddObjectTypesToQuery(ECC_WorldDynamic);
	AimTraceQueryObjectSettings.AddObjectTypesToQuery(ECC_Pawn);
	AimTraceQueryObjectSettings.AddObjectTypesToQuery(ECC_PhysicsBody);

	/* Ignore ourselves (we have ECC_Pawn enabled, but that's for hitting other pawns,
	 * such as enemies, not ourselves. You can specify general trace settings with the
	 * FCollisionQueryParams type, the ...ObjectQueryParams one is just for setting
	 * which object types to detect. */
	FCollisionQueryParams AimTraceQuerySettings;
	AimTraceQuerySettings.AddIgnoredActor(this);

	TraceEnd = GetWorld()->LineTraceSingleByObjectType(OutHit, TraceStart, TraceEnd, AimTraceQueryObjectSettings, AimTraceQuerySettings)
		? OutHit.ImpactPoint : TraceEnd;

	/* Draw the result of the line trace as a debug line that lasts for 2 seconds */
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 2.0f);

	/* Get the location of the hand socket. Sockets are added to skeletal meshes to mark positions,
	 * usually for attachment or for spawning things at said location e.g. bullets/projectiles */
	const FVector HandLocation = GetMesh()->GetSocketLocation(MuzzleName);

	const FTransform SpawnTransform = FTransform((TraceEnd - HandLocation).Rotation(), HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileTypeToSpawn, SpawnTransform, SpawnParams);
}

/* Each <Foo>Attack function plays its corresponding animation, then sets a timer via it's corresponding
 * timer handle, for its corresponding duration, which calls the corresponding callback. */
void ASCharacter::PrimaryAttack()
{
	/* play the animation asset assigned in the editor */
	PlayAttackEffects(PrimaryAttackAnim);

	/* Set a one-shot timer that will call PrimaryAttackCallback when it expires. */
	GetWorldTimerManager().SetTimer(PrimaryAttackTimerHandle, this, &ASCharacter::PrimaryAttackCallback, PrimaryAttackTimerDuration);
}

/* See PrimaryAttack for explanation */
void ASCharacter::SecondaryAttack()
{
	PlayAttackEffects(SecondaryAttackAnim);
	GetWorldTimerManager().SetTimer(SecondaryAttackTimerHandle, this, &ASCharacter::SecondaryAttackCallback, SecondaryAttackTimerDuration);
}

/* See PrimaryAttack for explanation */
void ASCharacter::UltimateAttack()
{
	PlayAttackEffects(UltimateAttackAnim);
	GetWorldTimerManager().SetTimer(UltimateAttackTimerHandle, this, &ASCharacter::UltimateAttackCallback, UltimateAttackTimerDuration);
}

// Called after the Timer referenced by PrimaryAttackTimerHandle expires, which takes PrimaryAttackTimerDuration seconds.
void ASCharacter::PrimaryAttackCallback()
{
	ensureAlways(PrimaryProjectileClass);
	Attack(PrimaryProjectileClass);
}


void ASCharacter::SecondaryAttackCallback()
{
	ensureAlways(SecondaryProjectileClass);
	Attack(SecondaryProjectileClass);
}


void ASCharacter::UltimateAttackCallback()
{
	ensureAlways(UltimateProjectileClass);
	Attack(UltimateProjectileClass);
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

