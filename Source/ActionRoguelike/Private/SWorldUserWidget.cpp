// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SizeBox.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/* If we don't have an actor assigned as our owner, then we don't have a position to be 
	 * rendered at (we use OwningActor->GetActorLocation() below). */
	if (!IsValid(OwningActor))
	{
		/* Remove this widget from the viewport (or its parent widget in the viewport) */
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("No owning actor set for world user widget."))
		return;
	}

	/* Get the screen position based on the real world location. */
	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), OwningActor->GetActorLocation() + WorldOffset, IN ScreenPosition))
	{
		/* Apply screen scaling to the ScreenPosition, as we may not be playing at 1920x1080 and that is what 
		 * it expects by default. GetViewportScale will return 1 if we are on 1920x1080, but could be greater
		 * or lesser for other screen sizes. */
		ScreenPosition /= UWidgetLayoutLibrary::GetViewportScale(GetWorld());
		ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
}
