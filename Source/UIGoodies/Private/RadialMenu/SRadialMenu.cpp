// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/SRadialMenu.h"
#include "Layout/LayoutUtils.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInterface.h"

#define LOCTEXT_NAMESPACE "UIGoodies"

SLATE_IMPLEMENT_WIDGET(SImage)
void SRadialMenu::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Image", BorderImageAttribute, EInvalidateWidgetReason::Layout);
}


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
	,BorderImageAttribute(*this, FCoreStyle::Get().GetBrush("Border"))
	, PreferredRadius(1.f)
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
	PreferredRadius = InArgs._PreferredRadius;
	StartingAngle = InArgs._StartingAngle;
	StickDeadzone = InArgs._StickDeadzone;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	OnAngleChanged = InArgs._OnAngleChanged;

	SetBorderImage(InArgs._BorderImage);

	Slots.AddSlots(MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)));
}

void SRadialMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	Width = AllottedGeometry.GetLocalSize().X;

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
			CurrentAngle = FRotator::ClampAxis(FMath::RadiansToDegrees(FMath::Atan2(AnalogValueTemp.Y, AnalogValueTemp.X)));
			OnAngleChanged.ExecuteIfBound(CurrentAngle);

			for (int32 ChildIndex = 0; ChildIndex < Slots.Num(); ++ChildIndex)
			{
				const FSlot& Slot = Slots[ChildIndex];

				float AngleDifference = Slot.GetAngle() - CurrentAngle;
				if (AngleDifference > 180) {
					AngleDifference -= 360;
				}
				else if (AngleDifference < -180) {
					AngleDifference += 360;
				}

				if (FMath::Abs(AngleDifference) <= Slot.GetAngleWidth() * 0.5)
				{
					if (SelectedSlot != ChildIndex)
					{
						SelectedSlot = ChildIndex;
						OnSelectionChanged.ExecuteIfBound(SelectedSlot);

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

SRadialMenu::FChildArranger::FChildArranger(const SRadialMenu& InRadialMenu, const FOnSlotArranged& InOnSlotArranged)
	: RadialMenu(InRadialMenu)
	, OnSlotArranged(InOnSlotArranged)
{
	OngoingArrangementDataMap.Reserve(RadialMenu.Slots.Num());
}

void SRadialMenu::FChildArranger::Arrange()
{
	const int32 NumItems = RadialMenu.Slots.Num();
	const float Radius = RadialMenu.Width / 2.f * RadialMenu.PreferredRadius;

	int32 TargetNumItems = FMath::Max(1, NumItems);
	const float DegreeIncrements = 360.0f/ TargetNumItems;

	//Offset to create the elements based on the middle of the widget as starting point
	const float MiddlePointOffset = RadialMenu.Width / 2.f;

	for (int32 ChildIndex = 0; ChildIndex < NumItems; ++ChildIndex)
	{
		const FSlot& Slot = RadialMenu.Slots[ChildIndex];
		const TSharedRef<SWidget>& Widget = Slot.GetWidget();

		// Skip collapsed widgets.
		if (Widget->GetVisibility() == EVisibility::Collapsed)
		{
			continue;
		}

		float DegreeOffset = -Slot.GetAngle();

		FArrangementData& ArrangementData = OngoingArrangementDataMap.Add(ChildIndex, FArrangementData());

		const FVector2D DesiredSizeOfSlot = Widget->GetDesiredSize();

		float SmallestSide = FMath::Min(DesiredSizeOfSlot.X / 2.f, DesiredSizeOfSlot.Y / 2.f);
		ArrangementData.SlotOffset.X = (Radius - SmallestSide) * Slot.GetDirection().X + MiddlePointOffset - DesiredSizeOfSlot.X / 2.f;
		ArrangementData.SlotOffset.Y = (Radius - SmallestSide) * Slot.GetDirection().Y + MiddlePointOffset - DesiredSizeOfSlot.Y / 2.f;
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

		FVector2D Direction;
		Direction.X = FMath::Cos(FMath::DegreesToRadians(-DegreeOffset));
		Direction.Y = FMath::Sin(FMath::DegreesToRadians(-DegreeOffset));

		Slot.SetDirection(Direction);

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

float SRadialMenu::GetSlotAngle(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		return Slots[SlotIndex].GetAngle();
	}

	return 0;
}

void SRadialMenu::SetBorderImage(TAttribute<const FSlateBrush*> InBorderImage)
{
	BorderImageAttribute.Assign(*this, InBorderImage);
}

void SRadialMenu::NotifySlotChanged(const FSlot* InSlot, bool bSlotLayerChanged)
{
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

FReply SRadialMenu::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

int32 SRadialMenu::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* BrushResource = BorderImageAttribute.Get();

	const bool bEnabled = ShouldBeEnabled(bParentEnabled);

	if (BrushResource && BrushResource->DrawAs != ESlateBrushDrawType::NoDrawType)
	{
		const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(1, 1)));
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			FlippedGeometry.ToPaintGeometry(),
			BrushResource,
			ESlateDrawEffect::None,
			BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
		);
	}

	return SPanel::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bEnabled);
}

#undef LOCTEXT_NAMESPACE