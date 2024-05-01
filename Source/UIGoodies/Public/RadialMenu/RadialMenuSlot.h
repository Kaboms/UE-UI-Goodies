// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "RadialMenu/SRadialMenu.h"

#include "RadialMenuSlot.generated.h"

/**
 * 
 */
UCLASS()
class UIGOODIES_API URadialMenuSlot : public UPanelSlot
{
	GENERATED_BODY()

public:
	URadialMenuSlot(const FObjectInitializer& ObjectInitializer);

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	// End of UPanelSlot interface

	/** Builds the underlying FSlot for the Slate layout panel. */
	void BuildSlot(TSharedRef<SRadialMenu> RadialMenu);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual void SynchronizeFromTemplate(const UPanelSlot* const TemplateSlot) override;
#endif //WITH_EDITOR

	UFUNCTION(BlueprintCallable, Category = "Layout|Radial Menu Slot")
	void SetWeight(float Weight);

	UFUNCTION(BlueprintGetter, Category = "Layout|Radial Menu Slot", meta = (ClampMin = 0, ClampMax = 1))
	float GetWeight();

protected:
	UPROPERTY(EditAnywhere, BlueprintSetter = "SetWeight", BlueprintGetter = "GetWeight", Category = "Layout|Radial Menu Slot")
	float Weight;

private:
	/** A raw pointer to the slot to allow us to adjust the size, padding...etc at runtime. */
	SRadialMenu::FSlot* Slot;
};
