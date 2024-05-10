// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/RadialMenu.h"
#include "RadialMenu/RadialMenuSlot.h"
#include "Editor/WidgetCompilerLog.h"
#include "Slate/SlateBrushAsset.h"

#define LOCTEXT_NAMESPACE "UIGoodies"

#define RADIALMENU_MATERIAL_SELECTEDSECTORANGLE "SelectedSectorAngle"
#define RADIALMENU_MATERIAL_SELECTORANGLE "SelectorAngle"
#define RADIALMENU_MATERIAL_SECTORSAMOUNT "SectorsAmount"

URadialMenu::URadialMenu(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	SetVisibilityInternal(ESlateVisibility::SelfHitTestInvisible);
}

void URadialMenu::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyRadialMenu.Reset();
}

TSharedRef<SWidget> URadialMenu::RebuildWidget()
{
	MyRadialMenu = SNew(SRadialMenu)
		.PreferredRadius(PreferredRadius)
		.StartingAngle(StartingAngle)
		.OnSelectionChanged(SRadialMenu::FOnSelectionChanged::CreateUObject(this, &URadialMenu::HandleOnSelectionChanged))
		.OnAngleChanged(SRadialMenu::FOnAngleChanged::CreateUObject(this, &URadialMenu::HandleOnAngleChanged))
		.CursorSpeed(CursorSpeed);

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (URadialMenuSlot* TypedSlot = Cast<URadialMenuSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyRadialMenu.ToSharedRef());
		}
	}

	if (!IsDesignTime())
	{
		if (BorderDynamicMaterial == nullptr)
		{
			UMaterialInstanceDynamic* const ParentMaterialDynamic = Cast<UMaterialInstanceDynamic>(Background.GetResourceObject());
			if (ParentMaterialDynamic == nullptr)
			{
				UMaterialInterface* ParentMaterial = Cast<UMaterialInterface>(Background.GetResourceObject());
				if (ParentMaterial)
				{
					BorderDynamicMaterial = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
					Background.SetResourceObject(BorderDynamicMaterial);

					BorderDynamicMaterial->SetScalarParameterValue(RADIALMENU_MATERIAL_SECTORSAMOUNT, Slots.Num());
				}
				else
				{
					BorderDynamicMaterial = nullptr;
				}
			}
		}

		MyRadialMenu->InitInputProcessor(bMouseAsAnalogCursor, StickType);
	}

    return MyRadialMenu.ToSharedRef();
}

void URadialMenu::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyRadialMenu.IsValid())
		return;

	MyRadialMenu->SetStartingAngle(StartingAngle);
	MyRadialMenu->SetBorderImage(&Background);
	MyRadialMenu->SetPreferredRadius(PreferredRadius);
	MyRadialMenu->SetMouseAsAnalogCursor(bMouseAsAnalogCursor);
	MyRadialMenu->SetCursorSpeed(CursorSpeed);
}

namespace DynamicRadialMenuCreateEntryInternal
{
	TArray<TSubclassOf<UUserWidget>, TInlineAllocator<4>> RecursiveDetection;
}

void URadialMenu::Reset(bool bDeleteWidgets)
{
    MyRadialMenu->ClearChildren();
}

int32 URadialMenu::GetSelectedSlot()
{
	if (MyRadialMenu.IsValid())
	{
		return MyRadialMenu->GetSelectedSlot();
	}
	return -1;
}

void URadialMenu::SetBrush(const FSlateBrush& InBrush)
{
	Background = InBrush;

	if (MyRadialMenu.IsValid())
	{
		MyRadialMenu->SetBorderImage(&Background);
	}
}

void URadialMenu::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	Background = Asset ? Asset->Brush : FSlateBrush();

	if (MyRadialMenu.IsValid())
	{
		MyRadialMenu->SetBorderImage(&Background);
	}
}

void URadialMenu::SetBrushFromTexture(UTexture2D* Texture)
{
	Background.SetResourceObject(Texture);

	if (MyRadialMenu.IsValid())
	{
		MyRadialMenu->SetBorderImage(&Background);
	}
}

void URadialMenu::SetBrushFromMaterial(UMaterialInterface* Material)
{
	if (!Material)
	{
		UE_LOG(LogSlate, Log, TEXT("URadialMenu::SetBrushFromMaterial. Incoming material is null. %s"), *GetPathName());
	}

	Background.SetResourceObject(Material);

	if (MyRadialMenu.IsValid())
	{
		MyRadialMenu->SetBorderImage(&Background);
	}
}

void URadialMenu::SelectSlot(int32 SlotIndex)
{
	if (MyRadialMenu.IsValid())
	{
		MyRadialMenu->SelectSlot(SlotIndex);
	}
}

UClass* URadialMenu::GetSlotClass() const
{
	return URadialMenuSlot::StaticClass();
}

void URadialMenu::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyRadialMenu.IsValid())
	{
		CastChecked<URadialMenuSlot>(InSlot)->BuildSlot(MyRadialMenu.ToSharedRef());
	}
}

void URadialMenu::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyRadialMenu.IsValid() && InSlot->Content)
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyRadialMenu->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

void URadialMenu::HandleOnSelectionChanged(int32 SlotIndex)
{
#if WITH_EDITOR
	// Slate emit this event from Tick. If broadcast event from tick it freeze the whole Editor UI, so we make async broadcast from GameThread
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnSelectionChanged.Broadcast(SlotIndex);
		});
#else
	OnSelectionChanged.Broadcast(SlotIndex);
#endif

	if (IsValid(BorderDynamicMaterial))
	{
		BorderDynamicMaterial->SetScalarParameterValue(RADIALMENU_MATERIAL_SELECTEDSECTORANGLE, MyRadialMenu->GetSlotAngle(SlotIndex));
	}
}

void URadialMenu::HandleOnAngleChanged(float Angle)
{
	if (IsValid(BorderDynamicMaterial))
	{
		BorderDynamicMaterial->SetScalarParameterValue(RADIALMENU_MATERIAL_SELECTORANGLE, Angle);
	}
}

#undef LOCTEXT_NAMESPACE