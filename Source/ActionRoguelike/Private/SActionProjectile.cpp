// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionProjectile.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


USActionProjectile::USActionProjectile()
{
	MuzzleName = "Muzzle_01";
	CastTimerDuration = 0.2f;
	AimTraceDistance = 5000;
}

void USActionProjectile::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	/* We need a reference to a Character if we want to call character specific functions... obviously. */
	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		/* Play the cast animation and VFX */
		Character->PlayAnimMontage(CastAnim);
		UGameplayStatics::SpawnEmitterAttached(
			CastEffect, Character->GetMesh(), MuzzleName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
		);

		if (Instigator->HasAuthority())
		{
			/* Start a timer for actually spawning the projectile once the casting animation is complete.
			 * Animation notifies would probably be better for this. */
			FTimerDelegate CastDelegate;
			CastDelegate.BindUFunction(this, "OnCastTimerElapsed", Character);
			GetWorld()->GetTimerManager().SetTimer(CastTimerHandle, CastDelegate, CastTimerDuration, false);
		}
	}
}

void USActionProjectile::OnCastTimerElapsed(ACharacter* InstigatorCharacter)
{
	ensure(ProjectileClass); 
	ensure(InstigatorCharacter); 
	ensure(InstigatorCharacter->HasAuthority());

	/* Perform a trace to try find the aim point. If nothing is hit, then just take
	 * the end of the trace as the aim point. */
	FHitResult OutHit;
	const FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
	FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * AimTraceDistance);
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
	AimTraceQuerySettings.AddIgnoredActor(InstigatorCharacter);

	TraceEnd = GetWorld()->LineTraceSingleByObjectType(OutHit, TraceStart, TraceEnd, AimTraceQueryObjectSettings, AimTraceQuerySettings)
		? OutHit.ImpactPoint : TraceEnd;

	/* Draw the result of the line trace as a debug line that lasts for 2 seconds */
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 2.0f);

	/* Get the location of the hand socket. Sockets are added to skeletal meshes to mark positions,
	 * usually for attachment or for spawning things at said location e.g. bullets/projectiles */
	const FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(MuzzleName);

	const FTransform SpawnTransform = FTransform((TraceEnd - HandLocation).Rotation(), HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = InstigatorCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParams);

	StopAction(InstigatorCharacter);
}
