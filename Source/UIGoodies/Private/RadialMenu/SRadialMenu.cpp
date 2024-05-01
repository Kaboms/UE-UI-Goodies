// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/SRadialMenu.h"
#include "Layout/LayoutUtils.h"
#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "UIGoodies"

void SRadialMenu::FSlot::Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs)
{
	TSlotBase<FSlot>::Construct(SlotOwner, MoveTemp(InArgs));
	if (InArgs._Weight.IsSet())
	{
		WeightParam = FMath::Clamp(InArgs._Weight.GetValue(), 0, 1);
	}
}

SRadialMenu::SRadialMenu()
	: Slots(this)
	, PreferredWidth(*this, 100.f)
{
}

SRadialMenu::FSlot::FSlotArguments SRadialMenu::Slot()
{
	return FSlot::FSlotArguments(MakeUnique<FSlot>());
}

SRadialMenu::FScopedWidgetSlotArguments SRadialMenu::AddSlot()
{
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(), Slots, INDEX_NONE };
}

int32 SRadialMenu::RemoveSlot(const TSharedRef<SWidget>& SlotWidget)
{
	return Slots.Remove(SlotWidget);
}

void SRadialMenu::Construct(const FArguments& InArgs)
{
	PreferredWidth.Assign(*this, InArgs._PreferredWidth);
	bUseAllottedWidth = InArgs._UseAllottedWidth;
	StartingAngle = InArgs._StartingAngle;
	StickDeadzone = InArgs._StickDeadzone;

	Slots.AddSlots(MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)));
}

PRAGMA_DISABLE_OPTIMIZATION
void SRadialMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUseAllottedWidth)
	{
		PreferredWidth.Set(*this, AllottedGeometry.GetLocalSize().X);
	}

	// Adjust analog values according to dead zone
	const float AnalogValsSize = AnalogValue.Size();
	FVector2D AnalogValueTemp = AnalogValue;
	if (AnalogValsSize > 0.0f)
	{
		const float TargetSize = FMath::Max(AnalogValsSize - StickDeadzone, 0.0f) / (1.0f - StickDeadzone);
		AnalogValueTemp /= AnalogValsSize;
		AnalogValueTemp *= TargetSize;

		if (AnalogValueTemp.Size() >= 0.05f)
		{
			double Angle = FRotator::ClampAxis(FMath::RadiansToDegrees(FMath::Atan2(AnalogValueTemp.Y, AnalogValueTemp.X)));

			for (int32 ChildIndex = 0; ChildIndex < Slots.Num(); ++ChildIndex)
			{
				const FSlot& Slot = Slots[ChildIndex];
				if (FMath::Abs(FRotator::ClampAxis(Slot.GetAngle() - Angle)) <= Slot.GetAngleWidth() * 0.5)
				{
					if (SelectedSlot != ChildIndex)
					{
						SelectedSlot = ChildIndex;

						FSlateApplication::Get().ForEachUser([&](FSlateUser& User) {
							if (FSlateApplication::Get().SetUserFocus(User.GetUserIndex(), Slot.GetWidget(), EFocusCause::SetDirectly))
							{
							}
							});
					}
					break;
				}
			}
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION

/*
 * Simple class for handling the circular arrangement of elements
 */
class SRadialMenu::FChildArranger
{
public:
	struct FArrangementData
	{
		FVector2D SlotOffset;
		FVector2D SlotSize;
	};

	typedef TFunctionRef<void(const FSlot& Slot, const FArrangementData& ArrangementData)> FOnSlotArranged;

	static void Arrange(const SRadialMenu& RadialMenu, const FOnSlotArranged& OnSlotArranged);

private:
	FChildArranger(const SRadialMenu& RadialMenu, const FOnSlotArranged& OnSlotArranged);
	void Arrange();

	const SRadialMenu& RadialMenu;
	const FOnSlotArranged& OnSlotArranged;
	TMap<int32, FArrangementData> OngoingArrangementDataMap;
};


SRadialMenu::FChildArranger::FChildArranger(const SRadialMenu& InRadialMenu, const FOnSlotArranged& InOnSlotArranged)
	: RadialMenu(InRadialMenu)
	, OnSlotArranged(InOnSlotArranged)
{
	OngoingArrangementDataMap.Reserve(RadialMenu.Slots.Num());
}

void SRadialMenu::FChildArranger::Arrange()
{
	const int32 NumItems = RadialMenu.Slots.Num();
	const float Radius = RadialMenu.PreferredWidth.Get() / 2.f;

	int32 TargetNumItems = FMath::Max(1, NumItems);
	const float DegreeIncrements = 360.0f/ TargetNumItems;

	//Offset to create the elements based on the middle of the widget as starting point
	const float MiddlePointOffset = RadialMenu.PreferredWidth.Get() / 2.f;

	for (int32 ChildIndex = 0; ChildIndex < NumItems; ++ChildIndex)
	{
		const FSlot& Slot = RadialMenu.Slots[ChildIndex];
		const TSharedRef<SWidget>& Widget = Slot.GetWidget();

		float DegreeOffset = -Slot.GetAngle();

		// Skip collapsed widgets.
		if (Widget->GetVisibility() == EVisibility::Collapsed)
		{
			continue;
		}

		FArrangementData& ArrangementData = OngoingArrangementDataMap.Add(ChildIndex, FArrangementData());

		const FVector2D DesiredSizeOfSlot = Widget->GetDesiredSize();

		float SmallestSide = FMath::Min(DesiredSizeOfSlot.X / 2.f, DesiredSizeOfSlot.Y / 2.f);
		ArrangementData.SlotOffset.X = (Radius - SmallestSide) * FMath::Cos(FMath::DegreesToRadians(DegreeOffset)) + MiddlePointOffset - DesiredSizeOfSlot.X / 2.f;
		ArrangementData.SlotOffset.Y = (Radius - SmallestSide) * FMath::Sin(FMath::DegreesToRadians(DegreeOffset)) + MiddlePointOffset - DesiredSizeOfSlot.Y / 2.f;
		ArrangementData.SlotSize.X = DesiredSizeOfSlot.X;
		ArrangementData.SlotSize.Y = DesiredSizeOfSlot.Y;

		OnSlotArranged(Slot, ArrangementData);
	}
}

void SRadialMenu::FChildArranger::Arrange(const SRadialMenu& RadialMenu, const FOnSlotArranged& OnSlotArranged)
{
	FChildArranger(RadialMenu, OnSlotArranged).Arrange();
}

void SRadialMenu::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	FChildArranger::Arrange(*this, [&](const FSlot& Slot, const FChildArranger::FArrangementData& ArrangementData)
		{
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(Slot.GetWidget(), ArrangementData.SlotOffset, ArrangementData.SlotSize));
		});
}

void SRadialMenu::CacheDesiredSize(float LayoutScaleMultiplier)
{
	TotalWeight = 0;

	float DegreeOffset = StartingAngle;

	for (int32 ChildIndex = 0; ChildIndex < Slots.Num(); ++ChildIndex)
	{
		FSlot& Slot = Slots[ChildIndex];
		TotalWeight += Slot.GetWeight();
	}

	for (int32 ChildIndex = 0; ChildIndex < Slots.Num(); ++ChildIndex)
	{
		FSlot& Slot = Slots[ChildIndex];

		Slot.SetAngle(DegreeOffset);

		float SlotAngleSizeAlpha = (Slot.GetWeight() / TotalWeight);
		Slot.SetAngleWidth(360 * SlotAngleSizeAlpha);

		DegreeOffset += Slot.GetAngleWidth();
	}
}

void SRadialMenu::ClearChildren()
{
	Slots.Empty();
}

FVector2D SRadialMenu::ComputeDesiredSize(float) const
{
	FVector2D MyDesiredSize = FVector2D::ZeroVector;

	FChildArranger::Arrange(*this, [&](const FSlot& Slot, const FChildArranger::FArrangementData& ArrangementData)
		{
			// Increase desired size to the maximum X and Y positions of any child widget.
			MyDesiredSize.X = FMath::Max(MyDesiredSize.X, ArrangementData.SlotOffset.X + ArrangementData.SlotSize.X);
			MyDesiredSize.Y = FMath::Max(MyDesiredSize.Y, ArrangementData.SlotOffset.Y + ArrangementData.SlotSize.Y);
		});

	return MyDesiredSize;
}

FChildren* SRadialMenu::GetChildren()
{
	return &Slots;
}

void SRadialMenu::SetUseAllottedWidth(bool bInUseAllottedWidth)
{
	bUseAllottedWidth = bInUseAllottedWidth;
}

FReply SRadialMenu::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
	FKey Key = InAnalogInputEvent.GetKey();
	float InputAnalogValue = InAnalogInputEvent.GetAnalogValue();

	if (Key == EKeys::Gamepad_LeftX)
	{
		AnalogValue.X = InputAnalogValue;
	}
	else if (Key == EKeys::Gamepad_LeftY)
	{
		AnalogValue.Y = InputAnalogValue;
	}

	return FReply::Handled();
}

void SRadialMenu::NotifySlotChanged(const FSlot* InSlot, bool bSlotLayerChanged)
{
}

#undef LOCTEXT_NAMESPACE