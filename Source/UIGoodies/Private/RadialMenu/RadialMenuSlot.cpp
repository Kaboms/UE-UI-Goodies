// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu/RadialMenuSlot.h"

URadialMenuSlot::URadialMenuSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{
    Weight = 1.0f;
}

void URadialMenuSlot::SynchronizeProperties()
{
    SetWeight(Weight);
}

void URadialMenuSlot::BuildSlot(TSharedRef<SRadialMenu> RadialMenu)
{
    RadialMenu->AddSlot()
        .Expose(Slot)
        .Weight(Weight)
        [
            Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
        ];
}

void URadialMenuSlot::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    Slot = nullptr;
}

void URadialMenuSlot::SynchronizeFromTemplate(const UPanelSlot* const TemplateSlot)
{
    const ThisClass* const TemplateRadialMenuSlot = CastChecked<ThisClass>(TemplateSlot);
    SetWeight(TemplateRadialMenuSlot->Weight);
}

void URadialMenuSlot::SetWeight(float InWeight)
{
    Weight = InWeight;
    if (Slot)
    {
        Slot->SetWeight(Weight);
    }
}

float URadialMenuSlot::GetWeight()
{
    return Slot ? Slot->GetWeight() : Weight;
}
