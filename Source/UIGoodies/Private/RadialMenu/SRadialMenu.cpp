// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/SRadialMenu.h"
#include "Layout/LayoutUtils.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInterface.h"
#include "Framework/Application/SlateUser.h"

SLATE_IMPLEMENT_WIDGET(SRadialMenu)
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
{
}

void SRadialMenu::Construct(const FArguments& InArgs)
{
	TotalWeight = 0;

	PreferredRadius = InArgs._PreferredRadius;
	StartingAngle = InArgs._StartingAngle;
	AnalogValueDeadzone = InArgs._AnalogValueDeadzone;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	OnAngleChanged = InArgs._OnAngleChanged;
	CursorSpeed = InArgs._CursorSpeed;

	SetBorderImage(InArgs._BorderImage);

	Slots.AddSlots(MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)));
}

void SRadialMenu::InitInputProcessor(bool UseMouseAsAnalogCursor, EAnalogStickType StickType)
{
	if (!FSlateApplication::IsInitialized())
		return;

	InputProcessor = MakeShared<FRadialMenuInputProcessor>(SharedThis(this));
	InputProcessor->SetMouseAsAnalogCursor(UseMouseAsAnalogCursor);
	InputProcessor->SetAnalogStickType(StickType);

	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);
}

SRadialMenu::~SRadialMenu()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}
}

SRadialMenu::FSlot::FSlotArguments SRadialMenu::Slot()
{
	return FSlot::FSlotArguments(MakeUnique<FSlot>());
}

SRadialMenu::FScopedWidgetSlotArguments SRadialMenu::AddSlot()
{
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(), Slots, INDEX_NONE };
}

void SRadialMenu::OnSlotAdded(int32 Index)
{
	FSlot& NewSlot = Slots[Index];
	TotalWeight += NewSlot.GetWeight();

	float DegreeOffset = StartingAngle;

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

int32 SRadialMenu::RemoveSlot(const TSharedRef<SWidget>& SlotWidget)
{
	for (int32 SlotIdx = 0; SlotIdx < Slots.Num(); ++SlotIdx)
	{
		const FSlot& Slot = Slots[SlotIdx];
		if (SlotWidget == Slot.GetWidget())
		{
			TotalWeight -= Slot.GetWeight();

			Slots.RemoveAt(SlotIdx);
			return SlotIdx;
		}
	}

	return -1;
}

void SRadialMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	Width = AllottedGeometry.GetLocalSize().X;

	if (!InputProcessor.IsValid())
		return;

	FVector2D AnalogValueTemp = FVector2D::Zero();

	if (InputProcessor->HasAnalogInput())
	{
		AnalogValueTemp = InputProcessor->GetAnalogValue();
	}
	else if (!InputProcessor->GetMouseAsAnalogCursor() && InputProcessor->HasMouseInput())
	{
		FVector2D LocalPosition = AllottedGeometry.AbsoluteToLocal(InputProcessor->GetMousePosition());
		FVector2D CenterPosition = AllottedGeometry.GetLocalSize() / 2;

		AnalogValueTemp = LocalPosition - CenterPosition;
		AnalogValueTemp.Normalize();
	}

	// Adjust analog values according to dead zone
	const float AnalogValsSize = AnalogValueTemp.Size();

	if (AnalogValsSize > 0.0f)
	{
		const float TargetSize = FMath::Max(AnalogValsSize - AnalogValueDeadzone, 0.0f) / (1.0f - AnalogValueDeadzone);
		AnalogValueTemp /= AnalogValsSize;
		AnalogValueTemp *= TargetSize;

		if (AnalogValueTemp.Size() >= 0.05f)
		{
			TargetAngle = FRotator::ClampAxis(FMath::RadiansToDegrees(FMath::Atan2(-AnalogValueTemp.Y, AnalogValueTemp.X)));
		}
	}

	if (!FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 1))
	{
		CurrentAngle = FMath::FInterpTo(CurrentAngle, CurrentAngle + FRotator::NormalizeAxis(TargetAngle - CurrentAngle), InDeltaTime, CursorSpeed);
		OnAngleChanged.ExecuteIfBound(CurrentAngle);

		for (int32 ChildIndex = 0; ChildIndex < Slots.Num(); ++ChildIndex)
		{
			const FSlot& Slot = Slots[ChildIndex];

			float AngleDifference = FRotator::NormalizeAxis(Slot.GetAngle() - FRotator::NormalizeAxis(CurrentAngle));

			if (FMath::Abs(AngleDifference) <= Slot.GetAngleWidth() * 0.5)
			{
				SetSelectedSlot(ChildIndex);
				break;
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

void SRadialMenu::SetMouseAsAnalogCursor(bool InMouseAsAnalogCursor)
{
	if (InputProcessor.IsValid())
	{
		InputProcessor->SetMouseAsAnalogCursor(InMouseAsAnalogCursor);
	}
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

void SRadialMenu::SelectSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex))
		return;

	const FSlot& Slot = Slots[SlotIndex];

	CurrentAngle = Slot.GetAngle();
	TargetAngle = CurrentAngle;
	OnAngleChanged.ExecuteIfBound(CurrentAngle);

	SetSelectedSlot(SlotIndex);
}

void SRadialMenu::SetSelectedSlot(int32 SlotIndex)
{
	if (SelectedSlot != SlotIndex)
	{
		SelectedSlot = SlotIndex;
		OnSelectionChanged.ExecuteIfBound(SelectedSlot);

		FSlateApplication::Get().ForEachUser([&](FSlateUser& User) {
			if (FSlateApplication::Get().SetUserFocus(User.GetUserIndex(), Slots[SelectedSlot].GetWidget(), EFocusCause::SetDirectly))
			{
			}
			});
	}
}

void SRadialMenu::NotifySlotChanged(const FSlot* InSlot, bool bSlotLayerChanged)
{
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