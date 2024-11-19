// Fill out your copyright notice in the Description page of Project Settings.

#include "AGV/AGVDataManager.h"
#include "AGV/AGVDataContainer.h"
#include "JsonUtilities.h"

// ����ü �Է¹޾� �����̳� �ν��Ͻ�
UAGVDataContainer* UAGVDataManager::CreateDataContainer(UObject* Outer, const FAGVData& InputData)
{
	UAGVDataContainer* NewContainer = NewObject<UAGVDataContainer>(Outer);
	if (NewContainer)
	{
		NewContainer->SetAGVData(InputData);
	}
	return NewContainer;
}

// Json ������Ʈ ���޹޾� ����ü�� ��ȯ
FAGVData UAGVDataManager::CreateDataContainer(const TSharedPtr<FJsonObject> OriginObject)
{	
	FAGVData NewData = FAGVData();
	if (!OriginObject)
	{
		UE_LOG(LogTemp, Error, TEXT("AGVDataManager : Creating CreateDataContainer Failed"));
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(OriginObject.ToSharedRef(), &NewData))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert JSON to struct"));
	}

	return NewData;
}
