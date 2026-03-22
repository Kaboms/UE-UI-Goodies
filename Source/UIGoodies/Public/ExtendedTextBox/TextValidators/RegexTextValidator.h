// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextValidatorBase.h"
#include "Internationalization/Regex.h"
#include "RegexTextValidator.generated.h"

/**
 *
 */
UCLASS()
class UIGOODIES_API URegexTextValidator : public UTextValidatorBase
{
	GENERATED_BODY()

public:
	virtual bool ValidateText(const FText& InText, FText& OutErrorMessage) override
	{
		if (!FRegexMatcher(FRegexPattern(RegexPattern), InText.ToString()).FindNext())
		{
			OutErrorMessage = ErrorMessage;
			return false;
		}
		return true;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RegexPattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ErrorMessage;
};