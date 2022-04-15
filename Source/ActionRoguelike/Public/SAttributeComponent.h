// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

/* As the macro name suggests, this declares a new type of delegate (event), specifically a
 * dynamic (can be serialized and have its bound functions found by name) multicast (can have
 * many functions bound to it, but no return value) delegate with four parameters. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnHealthChanged, AActor*, InstigatorActor, class USAttributeComponent*, OwningComponent, float, NewHealth, float, Delta
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	/* Static functions */

	/* The fact that we're passing a class reference to a static function may seem
	 * weird at first, you may be thinking, just make it a member function if you're
	 * passing an instance. However, the reason we do this is because we don't actually
	 * know whether it has an attributes component, and so we call getcomponentbyclass
	 * cast and return. It's basically a helper to not have to type that out. The 
	 * alternative to doing this static function is to create a small interface that
	 * has a GetAttributeComp method on it, and then any actor that has an attribute
	 * comp should implement that interface to return its attribute comp. */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributeComponent(AActor* FromActor);

	/* IMO this function is plain bad as we have to return something in the case that
	 * FromActor doesn't have an attributes component, and there's no objectively 
	 * correct answer about whether we should return true (alive) or false (dead)
	 * for such actors. I'm only adding it because Tom added it to the course and so
	 * we may need it in later classes. */
	UFUNCTION(BlueprintCallable, Category = "Attributes", meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(AActor* FromActor);

	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float DeltaHealth);

	/* This is an actual instance of the FOnHealthChanged delegate/event/broadcaster
	 * to which we can bind to via blueprints. Even if it was in the protected
	 * section, because it is BlueprintAssignable, we could still assign to this
	 * delegate from outside the class. For example, we got a reference to the
	 * player pawn in the health widget, then got a reference to this component
	 * via the pawn reference, then called the Assign On Health Change event
	 * node to assign a new red blueprint event node to this delegate. Note that
	 * this is an instance of the delegate, and other instances calling broadcast
	 * wont cause this one's subscribers to be notified - they are separate. */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;

	UFUNCTION(BlueprintPure)
	float GetHealth() const { return Health; };

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; };

	UFUNCTION(BlueprintPure)
	bool IsFullHealth() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health")
	float MaxHealth;

private:	

		
};
