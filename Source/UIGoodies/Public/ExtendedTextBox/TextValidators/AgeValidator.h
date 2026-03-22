// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DateTextValidator.h"
#include "AgeValidator.generated.h"

/**
 * Validate text lenght in range
 */
UCLASS()
class UIGOODIES_API UAgeValidator : public UDateTextValidator
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
        FDateTime AgeDateTime;
        AgeDateTime += (FDateTime::Now() - ResultDateTime);
        int32 Age = AgeDateTime.GetYear() - 1;

        if (Age < MinAge)
        {
            OutErrorMessage = MinAgeErrorMessage;
            return false;
        }

        if (Age > MaxAge)
        {
            OutErrorMessage = MaxAgeErrorMessage;
            return false;
        }

        return true;
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxAge = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText MaxAgeErrorMessage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MinAge = 18;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText MinAgeErrorMessage;
};