// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextValidatorBase.h"
#include "NotEmptyTextValidator.generated.h"

/**
 *
 */
UCLASS()
class UIGOODIES_API UNotEmptyTextValidator : public UTextValidatorBase
{
	GENERATED_BODY()

public:
	virtual bool ValidateText(const FText& InText, FText& OutErrorMessage) override
	{
		if (InText.IsEmpty())
		{
			OutErrorMessage = ErrorMessage;
			return false;
		}
		return true;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ErrorMessage;
};