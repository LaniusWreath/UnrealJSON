// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMWidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "Datas/JCMLog.h"

// ���� Ŭ���� �̸����� ĳ��
UUserWidget* UJCMWidgetManager::CreateWidgetFromClass(TSubclassOf<UUserWidget> WidetClass, FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		return WidgetMap[WidgetName];
	}

	if (!WidetClass)
	{
		UE_LOG(JCMlog, Error, TEXT("Invalid Widget Class"));
		return nullptr;
	}

	// ���� ����
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), WidetClass);
	if (NewWidget)
	{
		WidgetMap.Emplace(WidgetName, NewWidget);
	}

	return NewWidget;
}

// ���� ����Ʈ�� �߰�
void UJCMWidgetManager::ShowWidget(FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = WidgetMap[WidgetName];
		if (TargetWidget && !TargetWidget->IsInViewport())
		{
			TargetWidget->AddToViewport();
		}
	}
}

// ���� ����Ʈ���� ����
void UJCMWidgetManager::HideWidget(FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = WidgetMap[WidgetName];
		if (TargetWidget && TargetWidget->IsInViewport())
		{
			TargetWidget->RemoveFromViewport();
		}
	}
}
