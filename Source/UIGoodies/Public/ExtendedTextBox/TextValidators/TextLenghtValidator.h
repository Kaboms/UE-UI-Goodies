// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InRangeTextValidator.h"
#include "TextLenghtValidator.generated.h"

/**
 * Validate text lenght in range
 */
UCLASS()
class UIGOODIES_API UTextLenghtValidator : public UInRangeTextValidator
{
	GENERATED_BODY()

public:
	virtual bool ValidateText(const FText& InText, FText& OutErrorMessage) override
	{
		return ValidateValue(InText.ToString().Len(), OutErrorMessage);
	}
};