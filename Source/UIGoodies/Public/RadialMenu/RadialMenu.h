// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "Components/RadialBoxSettings.h"
#include "Blueprint/UserWidgetPool.h"

#include "RadialMenu/SRadialMenu.h"

#include "RadialMenu.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class USlateBrushAsset;
class UTexture2D;

/**
 * UMG Radial menu used slate SRadialMenu
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

	// Return -1 if no slot selected
	UFUNCTION(BlueprintPure)
	int32 GetSelectedSlot();

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetBrush(const FSlateBrush& Brush);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetBrushFromAsset(USlateBrushAsset* Asset);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetBrushFromTexture(UTexture2D* Texture);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetBrushFromMaterial(UMaterialInterface* Material);

	UFUNCTION(BlueprintCallable)
	void SelectSlot(int32 SlotIndex);

protected:
	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	void HandleOnSelectionChanged(int32 SlotIndex);
	void HandleOnAngleChanged(float Angle);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, int32, SelectedChild);

	UPROPERTY(BlueprintAssignable)
	FOnSelectionChanged OnSelectionChanged;

	/** Brush to drag as the background */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (DisplayName = "Brush"))
	FSlateBrush Background;

protected:
	/** Settings only relevant to RadialBox */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 360))
	float StartingAngle;

	// Preferred radius from 0 to 1.
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 1))
	float PreferredRadius = 1;

	UPROPERTY(EditAnywhere)
	bool bMouseAsAnalogCursor = true;

	// Set to 0 to instant cursor movement
	UPROPERTY(EditAnywhere)
	float CursorSpeed = 10;

	UPROPERTY(EditAnywhere)
	EAnalogStickType StickType;

	TSharedPtr<SRadialMenu> MyRadialMenu;

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> BorderDynamicMaterial;

};
