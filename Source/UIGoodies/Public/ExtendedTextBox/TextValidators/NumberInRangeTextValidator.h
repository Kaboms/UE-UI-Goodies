// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InRangeTextValidator.h"
#include "NumberInRangeTextValidator.generated.h"

/**
 * Validate number in range
 */
UCLASS()
class UIGOODIES_API UNumberInRangeTextValidator : public UInRangeTextValidator
{
	GENERATED_BODY()

public:
	virtual bool ValidateText(const FText& InText, FText& OutErrorMessage) override
	{
		if (InText.IsNumeric())
		{
			return ValidateValue(FCString::Atoi(*InText.ToString()), OutErrorMessage);
		}
		return true;
	}
};
