// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"
#include "InputCoreTypes.h"

class SRadialMenu;

/*
* Input processor for the radial menu.
* Widget can handle the input if it has a focus or a mouse hover.
* We should handle the input always if the radial menu is opened so we use InputProcessor for it.
*/
class FRadialMenuInputProcessor : public IInputProcessor
{

public:
	FRadialMenuInputProcessor(TSharedPtr<SRadialMenu> InOwner)
	{
		Owner = InOwner;
	}

	void SetOwner(TSharedPtr<SRadialMenu> InOwner)
	{
		Owner = InOwner;
	}

	virtual ~FRadialMenuInputProcessor() = default;

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
	}

	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override
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

		bHasInput = true;

		return true;
	}

	/** Mouse movement input */
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		MousePosition = MouseEvent.GetScreenSpacePosition();

		return true;
	}

	FVector2D GetAnalogValue()
	{
		bHasInput = false;

		return AnalogValue;
	}

	FVector2D GetMousePosition() const
	{
		return MousePosition;
	}

	bool HasInput() const
	{
		return bHasInput;
	}

private:
	TWeakPtr<SRadialMenu> Owner;

	FVector2D AnalogValue;

	FVector2D MousePosition;

	bool bHasInput;
};
