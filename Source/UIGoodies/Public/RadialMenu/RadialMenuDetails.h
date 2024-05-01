// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

class IPropertyHandle;
class IDetailCategoryBuilder;
class URadialMenu;

//////////////////////////////////////////////////////////////////////////
// FRadialMenuDetails
//////////////////////////////////////////////////////////////////////////

class FRadialMenuDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	/* Main customization of details */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

protected:
	TWeakObjectPtr<URadialMenu> RadialMenu;
};