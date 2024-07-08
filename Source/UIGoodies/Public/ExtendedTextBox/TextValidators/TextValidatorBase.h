// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextValidatorBase.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class UIGOODIES_API UTextValidatorBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual bool ValidateText(const FText& InText, FText& OutErrorMessage)
	{
		FString String = InText.ToString();
		return ReceiveValidateText(InText, OutErrorMessage);
	}

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Validate Text")
	bool ReceiveValidateText(const FText& InText, FText& OutErrorMessage);
};