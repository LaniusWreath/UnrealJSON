// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "HttpResponseWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UHttpResponseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddUser(FString Username);

	UFUNCTION(BlueprintCallable)
	void ClearUserList();

	UFUNCTION(BlueprintCallable)
	void ShowLoading(bool bIsLoading);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UVerticalBox* UserListBox;
};
