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
		RequestManagerInstance = NewObject<UJCMHttpHandler>(this);
	}
	return RequestManagerInstance;
}

UJCMCore* UJCMCore::GetJCMCore()
{
	// SingletonInstance�� ������ �����ϰ� �ʱ�ȭ
	if (!JCMCoreInstance)
	{
		JCMCoreInstance = NewObject<UJCMCore>();
		JCMCoreInstance->AddToRoot();  // GC ����
	}
	return JCMCoreInstance;
}

UJCMDataManager* UJCMCore::GetJCMDataManager()
{
	// DataManager �ν��Ͻ� ���� �� �ʱ�ȭ
	if (!DataManagerInstance)
	{
		DataManagerInstance = NewObject<UJCMDataManager>(this);
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
