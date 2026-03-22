// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtendedTextBox/ExtendedEditableTextBox.h"
#include "Components/TextBlock.h"
#include "ExtendedTextBox/TextValidators/TextValidatorBase.h"

void UExtendedEditableTextBox::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    NormalWidgetStyle = WidgetStyle;
    bInitialized = true;
}

void UExtendedEditableTextBox::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();

    if (!IsDesignTime())
    {
        // Notify the widget that it has been constructed.
        Construct();
    }
}

bool UExtendedEditableTextBox::IsTextValid()
{
    return CheckValidators(TextChangedValidators, GetText()) && CheckValidators(TextCommittedValidators, GetText());
}

void UExtendedEditableTextBox::HandleOnTextChanged(const FText& InText)
{
    Super::HandleOnTextChanged(InText);

    CheckValidators(TextChangedValidators, InText);

    if (!CheckValidators(AllowedTextValidators, InText, false) && !InText.IsEmpty())
    {
        // Stupid but most simple working hack
        SetText(PreviousText);
    }

    PreviousText = GetText();
}

void UExtendedEditableTextBox::HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod)
{
    FText TrimText = FText::TrimTrailing(FText::TrimPreceding(InText));
    SetText(TrimText);

    if (CheckValidators(TextChangedValidators, TrimText, false))
    {
        CheckValidators(TextCommittedValidators, TrimText);
    }

    Super::HandleOnTextCommitted(TrimText, CommitMethod);

    ReceiveHandleOnTextCommitted(TrimText, CommitMethod);
}

bool UExtendedEditableTextBox::CheckValidators(TArray<UTextValidatorBase*> Validators, const FText& InText, bool bErrorNotify)
{
    for (UTextValidatorBase* TextValidator : Validators)
    {
        FText ErrorMessage;
        if (!TextValidator->ValidateText(InText, ErrorMessage))
        {
            if (bErrorNotify)
            {
                ShowError(ErrorMessage);
            }
            // Emit only one error message
            return false;
        }
    }

    if (bErrorNotify)
    {
        HideError();
    }

    return true;
}

void UExtendedEditableTextBox::ShowError(FText ErrorText)
{
    if (!bInitialized)
        return;

    if (IsValid(ErrorMessageTextWidget))
    {
        ErrorMessageTextWidget->SetText(ErrorText);
    }

    WidgetStyle = ErrorWidgetStyle;
}

void UExtendedEditableTextBox::HideError()
{
    if (!bInitialized)
        return;

    if (IsValid(ErrorMessageTextWidget))
    {
        ErrorMessageTextWidget->SetText(FText::GetEmpty());
    }

    WidgetStyle = NormalWidgetStyle;
}