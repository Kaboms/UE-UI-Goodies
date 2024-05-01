// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "Components/RadialBoxSettings.h"
#include "Blueprint/UserWidgetPool.h"

#include "RadialMenu/SRadialMenu.h"

#include "RadialMenu.generated.h"

/**
 * 
 */
UCLASS()
class UIGOODIES_API URadialMenu : public UPanelWidget
{
	GENERATED_BODY()

public:
	URadialMenu(const FObjectInitializer& Initializer);
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;

	void Reset(bool bDeleteWidgets);

	//~ Begin UWidget Interface
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
	//~ End UWidget Interface

	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface
#endif

protected:
	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

protected:
	/** Settings only relevant to RadialBox */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 360))
	float StartingAngle;

	TSharedPtr<SRadialMenu> MyRadialMenu;
};
