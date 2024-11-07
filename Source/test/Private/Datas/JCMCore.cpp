// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JCMCore.h"
#include "Datas/JCMDataManager.h"
#include "Datas/JCMHttpHandler.h"

UJCMCore* UJCMCore::JCMCoreInstance = nullptr;

// RequestHandler�� �̱������� �Ŵ���ó�� ���
UJCMHttpHandler* UJCMCore::GetJCMRequestManager()
{
	if (!RequestManagerInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
	}
	return RequestManagerInstance;
}

UJCMCore* UJCMCore::GetJCMCore()
{
	if (JCMCoreInstance)
	{
		return JCMCoreInstance;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
		return nullptr;
	}
}

UJCMCore* UJCMCore::InitializeJCMCore()
{
	// SingletonInstance�� ������ �����ϰ� �ʱ�ȭ
	if (!JCMCoreInstance)
	{
		JCMCoreInstance = NewObject<UJCMCore>();
		JCMCoreInstance->AddToRoot();  // GC ����

		JCMCoreInstance->DataManagerInstance = NewObject<UJCMDataManager>();
		JCMCoreInstance->RequestManagerInstance = NewObject<UJCMHttpHandler>();
	}
	return JCMCoreInstance;
}

UJCMDataManager* UJCMCore::GetJCMDataManager()
{
	// DataManager �ν��Ͻ� ���� �� �ʱ�ȭ
	if (!DataManagerInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
	}
	return DataManagerInstance;
}

void UJCMCore::Destroy()
{
	if (JCMCoreInstance)
	{
		JCMCoreInstance->RemoveFromRoot();  // GC ���� ����
		JCMCoreInstance = nullptr;
	}
}
