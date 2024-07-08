// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextValidatorBase.h"
#include "InRangeTextValidator.generated.h"

/**
 *
 */
UCLASS(Abstract)
class UIGOODIES_API UInRangeTextValidator : public UTextValidatorBase
{
	GENERATED_BODY()

public:
	bool ValidateValue(int32 Value, FText& OutErrorMessage)
	{
		if (Value < MinValue)
		{
			OutErrorMessage = MinValueErrorMessage;
			return false;
		}
		if (Value > MaxValue)
		{
			OutErrorMessage = MaxValueErrorMessage;
			return false;
		}
		return true;
	}

public:
	// Min available value (inclusive)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MinValueErrorMessage;

	// Max available value (inclusive)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MaxValueErrorMessage;
};
