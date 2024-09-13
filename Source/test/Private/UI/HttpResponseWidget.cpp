// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HttpResponseWidget.h"
#include "Components/TextBlock.h"
#include "Components/CircularThrobber.h"
#include "Components/VerticalBoxSlot.h"


void UHttpResponseWidget::AddUser(FString Username)
{
	UTextBlock* TextBlock = NewObject<UTextBlock>();
	
	const FString Text = FString::Printf(TEXT("Username: %s"), *Username);

	TextBlock->SetText(FText::FromString(Text));

	UserListBox->AddChild(TextBlock);
}

void UHttpResponseWidget::ClearUserList()
{
	UserListBox->ClearChildren();
}

void UHttpResponseWidget::ShowLoading(bool bIsLoading)
{
	if(bIsLoading)
	{
		UCircularThrobber* LoadingIcon = NewObject<UCircularThrobber>();
		UPanelSlot* PanelSlot = UserListBox->AddChild(LoadingIcon);
		if (PanelSlot)
		{
			UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(PanelSlot);
			if (VerticalSlot)
			{
				VerticalSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			}
		}
	}
	else
	{
		ClearUserList();
	}
}
