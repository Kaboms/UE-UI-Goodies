// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/RadialMenu.h"
#include "RadialMenu/RadialMenuSlot.h"
#include "Editor/WidgetCompilerLog.h"

#define LOCTEXT_NAMESPACE "UIGoodies"

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
		.UseAllottedWidth(true)
		.StartingAngle(StartingAngle);

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (URadialMenuSlot* TypedSlot = Cast<URadialMenuSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyRadialMenu.ToSharedRef());
		}
	}

    return MyRadialMenu.ToSharedRef();
}

void URadialMenu::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITORONLY_DATA
	if (IsDesignTime() && MyRadialMenu.IsValid())
	{
		if (MyRadialMenu.IsValid())
		{
			MyRadialMenu->SetStartingAngle(StartingAngle);
		}
	}
#endif
}

namespace DynamicRadialMenuCreateEntryInternal
{
	TArray<TSubclassOf<UUserWidget>, TInlineAllocator<4>> RecursiveDetection;
}

void URadialMenu::Reset(bool bDeleteWidgets)
{
    MyRadialMenu->ClearChildren();
}

void URadialMenu::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
}

#if WITH_EDITOR
void URadialMenu::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
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
#endif

#undef LOCTEXT_NAMESPACE