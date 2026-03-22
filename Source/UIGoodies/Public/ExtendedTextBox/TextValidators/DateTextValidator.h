// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TextValidatorBase.h"
#include "DateTextValidator.generated.h"

/**
 * Validate text lenght in range
 */
UCLASS()
class UIGOODIES_API UDateTextValidator : public UTextValidatorBase
{
    GENERATED_BODY()

public:
    virtual bool ValidateText(const FText& InText, FText& OutErrorMessage) override
    {
        FDateTime ResultDateTime;

        if (!ParseDate(InText, ResultDateTime))
        {
            OutErrorMessage = ErrorMessage;
            return false;
        }

        return true;
    };

    bool ParseDate(const FText& InText, FDateTime& OutDate)
    {
        FString DateTimeString = InText.ToString();

        TArray<FString> Tokens;
        DateTimeString.ParseIntoArray(Tokens, TEXT("."), true);
        if (Tokens.Num() != 3)
        {
            return false;
        }

        DateTimeString = FString::Format(TEXT("{0}.{1}.{2}.0.0.0"), { Tokens[2], Tokens[1], Tokens[0] });

        if (!FDateTime::Parse(DateTimeString, OutDate))
        {
            return false;
        }

        return true;
    }


public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText ErrorMessage;
};