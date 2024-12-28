// Fill out your copyright notice in the Description page of Project Settings.

#include "AGV/AGVDataManager.h"
#include "JsonUtilities.h"
#include "AGV/AGVLog.h"
#include "AGV/AGVDataContainer.h"
#include "SFCommon/SFCHttpManager.h"
#include "SFCommon/SFCWebSocketManager.h"


//--------------------------------- Data Management --------------------------------------------

// Static ��ü
UAGVDataManager* UAGVDataManager::AGVDataManagerInstance = nullptr;

// ������ �Ŵ��� �ν��Ͻ� �ʱ�ȭ
const UAGVDataManager* UAGVDataManager::GetAGVDataManager()
{
	if (!AGVDataManagerInstance)
	{
		AGVDataManagerInstance = NewObject<UAGVDataManager>();
		AGVDataManagerInstance->AddToRoot();  // GC ����
	}
	return AGVDataManagerInstance;
}

// AGVData Json ������Ʈ ���޹޾� ����ü�� ��ȯ
FAGVData UAGVDataManager::JsonObjectToAGVStruct(const TSharedPtr<FJsonObject> OriginObject)
{
	FAGVData NewData = FAGVData();
	if (!OriginObject)
	{
		UE_LOG(AGVlog, Error, TEXT("AGVDataManager : Creating CreateDataContainer Failed"));
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(OriginObject.ToSharedRef(), &NewData))
	{
		UE_LOG(AGVlog, Error, TEXT("Failed to convert JSON to struct"));
	}

	return NewData;
}

FAGVData UAGVDataManager::JsonStringToAGVStruct(const FString& OriginString)
{
	FAGVData NewData = FAGVData();
	if (OriginString.IsEmpty())
	{
		UE_LOG(AGVlog, Error, TEXT("AGVDataManager : Creating CreateDataContainer Failed"));
	}

	if (!FJsonObjectConverter::JsonObjectStringToUStruct<FAGVData>(OriginString, &NewData))
	{
		UE_LOG(AGVlog, Error, TEXT("Failed to convert JSON to struct"));
	}

	return NewData;
}

// ����ü �Է¹޾� �����̳� �ν��Ͻ�
UAGVDataContainer* UAGVDataManager::InstancingDataContainer(UObject* Outer, const FAGVData& InputData)
{
	UAGVDataContainer* NewContainer = NewObject<UAGVDataContainer>(Outer);
	if (NewContainer)
	{
		NewContainer->SetAGVData(InputData);
	}
	return NewContainer;
}

// �� AGV ������ �����̳� ����
UAGVDataContainer* UAGVDataManager::CreateEmptyDataContainer(UObject* Outer)
{
	UAGVDataContainer* NewContainer = NewObject<UAGVDataContainer>(Outer);

	return NewContainer;
}