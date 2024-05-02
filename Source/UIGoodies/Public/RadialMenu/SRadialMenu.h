// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlotBase.h"
#include "Widgets/SWidget.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"

/**
 * 
 */
class UIGOODIES_API SRadialMenu : public SPanel
{

public:

	/** Basic Slot without padding or alignment */
	class FSlot : public TSlotBase<FSlot>
	{
		friend SRadialMenu;

	public:
		FSlot()
			: TSlotBase<FSlot>()
			, WeightParam(1.0f)
		{
		}

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
			SLATE_ARGUMENT(TOptional<float>, Weight)
		SLATE_SLOT_END_ARGS()

		void Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs);

		float GetWeight() const
		{
			return WeightParam;
		}

		void SetWeight(float Weight)
		{
			Weight = FMath::Max(0, Weight);
			if (Weight != WeightParam)
			{
				WeightParam = Weight;
				NotifySlotChanged();
			}
		}

		float GetAngle() const
		{
			return AngleParam;
		}

		void SetAngle(float Angle)
		{
			AngleParam = Angle;
		}

		float GetAngleWidth() const
		{
			return AngleWidthParam;
		}

		void SetAngleWidth(float AngleWidth)
		{
			AngleWidthParam = AngleWidth;
		}

	private:
		/** The panel that contains this slot */
		TWeakPtr<SRadialMenu> RadialMenu;

		float WeightParam;

		float AngleParam;
		float AngleWidthParam;

		/** Notify that the slot was changed */
		FORCEINLINE void NotifySlotChanged(bool bSlotLayerChanged = false)
		{
			if (RadialMenu.IsValid())
			{
				RadialMenu.Pin()->NotifySlotChanged(this, bSlotLayerChanged);
			}
		}
	};

	DECLARE_DELEGATE_OneParam(FOnSelectionChanged, int32);

	SLATE_BEGIN_ARGS(SRadialMenu)
		: _PreferredWidth(100.f)
		, _UseAllottedWidth(false)
		, _StartingAngle(0.f)
		, _StickDeadzone(0.5f)
		, _BorderImage(FCoreStyle::Get().GetBrush("Border"))
		{
			_Visibility = EVisibility::SelfHitTestInvisible;
		}

		/** The slot supported by this panel */
		SLATE_SLOT_ARGUMENT(FSlot, Slots)

		/** The preferred width, if not set will fill the space */
		SLATE_ATTRIBUTE(float, PreferredWidth)

		/** if true, the PreferredWidth will always match the room available to the SRadialMenu  */
		SLATE_ARGUMENT(bool, UseAllottedWidth)

		/** Offset of the first element in the circle in degrees */
		SLATE_ARGUMENT(float, StartingAngle)

		/** Analog value deadzone */
		SLATE_ARGUMENT(float, StickDeadzone)

		SLATE_ATTRIBUTE(const FSlateBrush*, BorderImage)

		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)

	SLATE_END_ARGS()

	SRadialMenu();

	static FSlot::FSlotArguments Slot();

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	FScopedWidgetSlotArguments AddSlot();

	/** Removes a slot from this radial box which contains the specified SWidget
	 *
	 * @param SlotWidget The widget to match when searching through the slots
	 * @returns The index in the children array where the slot was removed and -1 if no slot was found matching the widget
	 */
	int32 RemoveSlot(const TSharedRef<SWidget>& SlotWidget);

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	virtual void CacheDesiredSize(float LayoutScaleMultiplier) override;

	virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void ClearChildren();

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual FChildren* GetChildren() override;

	void SetStartingAngle(float InStartingAngle) { StartingAngle = InStartingAngle; }

	void SetStickDeadzone(float InStickDeadzone) { StickDeadzone = InStickDeadzone; }

	void SetUseAllottedWidth(bool bInUseAllottedWidth);

	// Return -1 if no slot selected
	int32 GetSelectedSlot() const { return SelectedSlot; }
	float GetCurrentAngle() const { return CurrentAngle; }

	/** Set the image to draw for this border. */
	void SetBorderImage(TAttribute<const FSlateBrush*> InBorderImage);

private:
	void NotifySlotChanged(const FSlot* InSlot, bool bSlotLayerChanged = false);

protected:
	/** The slots that contain this panel's children. */
	TPanelChildren<FSlot> Slots;

	int32 SelectedSlot = -1;

	TSlateAttribute<const FSlateBrush*> BorderImageAttribute;

	/** How wide this panel should appear to be. */
	TSlateAttribute<float, EInvalidateWidgetReason::Layout> PreferredWidth;

	/** Offset of the first element in the circle in degrees */
	float StartingAngle;

	float CurrentAngle;

	float StickDeadzone;

	/** If true the box will have a preferred width equal to its alloted width  */
	bool bUseAllottedWidth;

	FOnSelectionChanged OnSelectionChanged;

	class FChildArranger;
	friend class SRadialMenu::FChildArranger;

	FVector2D AnalogValue;

	float TotalWeight;
};
