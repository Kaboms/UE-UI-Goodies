// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "Styling/SlateBrush.h"
#include "ExtendedEditableTextBox.generated.h"

class UTextValidatorBase;

/**
 * 
 */
UCLASS()
class UIGOODIES_API UExtendedEditableTextBox : public UEditableTextBox
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

protected:
	virtual void OnWidgetRebuilt() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "User Interface", meta = (Keywords = "Begin Play"))
	void Construct();

	UFUNCTION(BlueprintPure)
	bool IsTextValid();

	virtual void HandleOnTextChanged(const FText& Text);
	virtual void HandleOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "HandleOnTextCommitted")
	void ReceiveHandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod);

	bool CheckValidators(TArray<UTextValidatorBase*> Validators, const FText& Text, bool bNotify = true);

	UFUNCTION(BlueprintCallable)
	void ShowError(FText ErrorText);

	UFUNCTION(BlueprintCallable)
	void HideError();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (DisplayName = "Error Style", ShowOnlyInnerProperties))
	FEditableTextBoxStyle ErrorWidgetStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTextBlock> ErrorMessageTextWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UTextValidatorBase*> TextChangedValidators;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UTextValidatorBase*> TextCommittedValidators;

	// Block all invalid text
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UTextValidatorBase*> AllowedTextValidators;


protected:
	UPROPERTY(BlueprintReadWrite)
	FEditableTextBoxStyle NormalWidgetStyle;

	bool bInitialized = false;

	FText PreviousText;
};
