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

		MouseDistance = FVector2D::Zero();
		MousePosition = FVector2D::Zero();
		LastMousePosition = FVector2D::Zero();
		AnalogValue = FVector2D::Zero();
	}

	void SetOwner(TSharedPtr<SRadialMenu> InOwner)
	{
		Owner = InOwner;
	}

	virtual ~FRadialMenuInputProcessor() = default;

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		if (bMouseAsAnalogCursor)
		{
			MouseDistance += MousePosition - LastMousePosition;
			if (MouseDistance.Size() > 50)
			{
				MouseDistance.Normalize();
				MouseDistance += AnalogValue;
				MouseDistance.Normalize();
				SetAnalogValue(MouseDistance);

				MouseDistance = FVector2D::Zero();
			}
			LastMousePosition = MousePosition;
		}
	}

	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override
	{
		FKey Key = InAnalogInputEvent.GetKey();
		float InputAnalogValue = InAnalogInputEvent.GetAnalogValue();

		if (StickType == EAnalogStickType::LeftStick)
		{
			if (Key == EKeys::Gamepad_LeftX)
			{
				AnalogValue.X = InputAnalogValue;
			}
			else if (Key == EKeys::Gamepad_LeftY)
			{
				AnalogValue.Y = -InputAnalogValue;
			}
		}
		else if (StickType == EAnalogStickType::RightStick)
		{
			if (Key == EKeys::Gamepad_RightX)
			{
				AnalogValue.X = InputAnalogValue;
			}
			else if (Key == EKeys::Gamepad_RightY)
			{
				AnalogValue.Y = -InputAnalogValue;
			}
		}
		bHasAnalogInput = true;

		return false;
	}

	/** Mouse movement input */
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		SetMousePosition(MouseEvent.GetScreenSpacePosition());

		return false;
	}

	FVector2D GetAnalogValue()
	{
		bHasAnalogInput = false;

		return AnalogValue;
	}

	void SetAnalogValue(FVector2D InAnalogInput)
	{
		bHasAnalogInput = true;
		AnalogValue = InAnalogInput;
	}

	void SetMousePosition(FVector2D InMousePosition)
	{
		bHasMouseInput = true;
		MousePosition = InMousePosition;
	}

	FVector2D GetMousePosition()
	{
		bHasMouseInput = false;
		return MousePosition;
	}

	bool HasAnalogInput() const
	{
		return bHasAnalogInput;
	}

	bool HasMouseInput()
	{
		return bHasMouseInput;
	}

	void SetMouseAsAnalogCursor(bool InMouseAsAnalogCursor)
	{
		bMouseAsAnalogCursor = InMouseAsAnalogCursor;
	}

	bool GetMouseAsAnalogCursor() const
	{
		return bMouseAsAnalogCursor;
	}

	void SetAnalogStickType(EAnalogStickType InStickType)
	{
		StickType = InStickType;
	}

private:
	TWeakPtr<SRadialMenu> Owner;

	EAnalogStickType StickType;

	FVector2D AnalogValue;

	FVector2D MousePosition;

	FVector2D LastMousePosition;

	FVector2D MouseDistance;

	bool bMouseAsAnalogCursor = false;

	bool bHasAnalogInput = false;
	bool bHasMouseInput = false;
};
