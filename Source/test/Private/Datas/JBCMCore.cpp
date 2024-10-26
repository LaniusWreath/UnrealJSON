// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JBCMCore.h"
#include "Datas/DataManager.h"
#include "Datas/HTTPRequestManager.h"

UJBCMCore* UJBCMCore::JBCMCoreInstance = nullptr;

UJBCMCore* UJBCMCore::GetJBCMCore()
{
	// SingletonInstance�� ������ �����ϰ� �ʱ�ȭ
	if (!JBCMCoreInstance)
	{
		JBCMCoreInstance = NewObject<UJBCMCore>();
		JBCMCoreInstance->AddToRoot();  // GC ����
		JBCMCoreInstance->InitializeManagers();
	}
	return JBCMCoreInstance;
}

void UJBCMCore::InitializeManagers()
{
	// �� Manager �ν��Ͻ� ���� �� �ʱ�ȭ
	RequestManagerInstance = NewObject<UHTTPRequestManager>(this);
	DataManagerInstance = NewObject<UDataManager>(this);
}

void UJBCMCore::Destroy()
{
	if (JBCMCoreInstance)
	{
		JBCMCoreInstance->RemoveFromRoot();  // GC ���� ����
		JBCMCoreInstance = nullptr;
	}
}
