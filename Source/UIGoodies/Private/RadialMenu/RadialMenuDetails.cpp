// Copyright Epic Games, Inc. All Rights Reserved.

#include "RadialMenu/RadialMenuDetails.h"

#include "DetailLayoutBuilder.h"
#include "RadialMenu/RadialMenu.h"
#include "PropertyCustomizationHelpers.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

//////////////////////////////////////////////////////////////////////////
// FRadialMenuDetails
//////////////////////////////////////////////////////////////////////////

TSharedRef<IDetailCustomization> FRadialMenuDetails::MakeInstance()
{
	return MakeShareable(new FRadialMenuDetails());
}

void FRadialMenuDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);
	if (Objects.Num() != 1)
	{
		return;
	}
	RadialMenu = Cast<URadialMenu>(Objects[0].Get());
	if (!RadialMenu.IsValid())
	{
		return;
	}

	//IDetailCategoryBuilder& EntryLayoutCategory = DetailLayout.EditCategory(TEXT("EntryLayout"));;
	//const TAttribute<bool> CanEditAignmentAttribute(this, &FDynamicEntryBoxDetails::CanEditAlignment);
	//EntryLayoutCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDynamicEntryBoxBase, EntryHorizontalAlignment)))
	//	.IsEnabled(CanEditAignmentAttribute);
	//EntryLayoutCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDynamicEntryBoxBase, EntryVerticalAlignment)))
	//	.IsEnabled(CanEditAignmentAttribute);

	//EntryLayoutCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDynamicEntryBoxBase, MaxElementSize)))
	//	.IsEnabled(TAttribute<bool>(this, &FDynamicEntryBoxDetails::CanEditMaxElementSize));
	//EntryLayoutCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDynamicEntryBoxBase, EntrySpacing)))
	//	.IsEnabled(TAttribute<bool>(this, &FDynamicEntryBoxDetails::CanEditEntrySpacing));
	//EntryLayoutCategory.AddProperty(DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDynamicEntryBoxBase, SpacingPattern)))
	//	.IsEnabled(TAttribute<bool>(this, &FDynamicEntryBoxDetails::CanEditSpacingPattern));
}