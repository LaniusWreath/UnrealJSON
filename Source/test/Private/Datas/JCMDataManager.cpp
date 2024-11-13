// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JCMDataManager.h"
#include "Datas/JCMJsonHandler.h"
#include "Datas/JCMDataContainer.h"
#include "Serialization/JsonWriter.h"
#include "Datas/JCMLog.h"


// json ���� ���� ���� �а� ó���ϴ� ��ƾ
UJCMDataContainer* UJCMDataManager::InstancingDataContainerFromLocalJson(const FString& FilePath)
{
	TSharedPtr<FJsonObject> Data = LoadDataFromJSON(FilePath);
	FDataInstancePair NewChartData = InstancingDataClass(Data);
	if (NewChartData.IsValid)
	{
		return NewChartData.DataInstance;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'InstancingDataContainerFromLocalJson()' Failed"));
		return nullptr;
	}
}

// json FString �о� ������ �����̳� ��ȯ���ִ� 
UJCMDataContainer* UJCMDataManager::InstancingDataContainerFromJsonString(const FString& JsonBody)
{
	TSharedPtr<FJsonObject> Data = DeserializeJsonStringToJsonObject(JsonBody);
	FDataInstancePair NewChartData = InstancingDataClass(Data);
	return NewChartData.DataInstance;
}

// FString���� Serialize�� Json���ڿ� ��ü�� �ٽ� ��ȯ
TSharedPtr<FJsonObject> UJCMDataManager::DeserializeJsonStringToJsonObject(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	// JSON ���ڿ��� FJsonObject�� �Ľ�
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		return JsonObject;
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("JCMDataManager : Failed to deserialize JSON string."));
		return nullptr;
	}
}

// Ž���� �н��� ���� �б�, ���߿� �����ε� �ϴ� �� �ϴ� JSON�� �Ѱܹ޴� �Լ� ���� ��
TSharedPtr<FJsonObject> UJCMDataManager::LoadDataFromJSON(const FString& FilePath)
{
	if (!JSONHandlerInstance)
	{
		JSONHandlerInstance = NewObject<UJCMJsonHandler>(this);
	}
	if (JSONHandlerInstance)
	{
		// JSONHandler���� ���� JSON ������ �״�� ������, �̰� �޴� �Լ��� ���� �и�
		const TSharedPtr<FJsonObject> ParsedData = JSONHandlerInstance->GetJsonObjectData(FilePath);

		DataString = SerializeJSONToString(ParsedData);
		return ParsedData;
	}
	else
	{
		return nullptr;
	}
}

void UJCMDataManager::LoadDataFromCSV(const FString& FilePath)
{
}

void UJCMDataManager::FetchDataFromHTTP(const FString& URL)
{
}

// Return JSON String Getter()
const FString& UJCMDataManager::GetJSONStringData() const
{
	if (DataString == "")
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : String Data is null"));
	}
	return DataString;
}

// Create Emtpy BarType Data Container Instance
UJCMDataContainer* UJCMDataManager::CreateEmptyDataContainer(EJCMChartTypes ChartType)
{
	switch (ChartType)
	{
	case None:
		break;
	case BAR:
		return NewObject<UJCMDataContainerBar>();
		break;
	case LINE:
		return NewObject<UJCMDataContainerLine>();
		break;
	case PIE:
		return NewObject<UJCMDataContainerPie>();
		break;
	case XY:
		return NewObject<UJCMDataContainerXY>();
		break;
	case XYZ:
		return NewObject<UJCMDataContainerXYZ>();
		break;
	case FREE:
		return NewObject<UJCMDataContainer>();
		break;
	default:
		break;
	}
	return nullptr;
}

// JSON -> FString
FString UJCMDataManager::SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);

	if (FJsonSerializer::Serialize(JSONObject.ToSharedRef(), Writer))
	{
		UE_LOG(JCMlog, Log, TEXT("JCMDataManager : JSONToString serialized"));
	}
	else
	{
		UE_LOG(JCMlog, Log, TEXT("JCMDataManager : SONToString serialize failed"));
	}

	return JsonString;
}

// ������ �Է� �޾� �Ľ�, DataClass ��ü ���� -> Chart
FDataInstancePair UJCMDataManager::InstancingDataClass(const TSharedPtr<FJsonObject> Data) 
{
	bool isFieldValid = true;

	if (!Data.IsValid())
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Input Data is invalid"));
		isFieldValid = false;
	}

	// �𸮾��� ����ó�� ��Ȱ��ȭ��. �ϳ� �� �ʵ� ��ȿ���� �˻�
	FString ChartType;
	if (!Data->TryGetStringField(TEXT("chartType"), ChartType))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'chartType' is missing or invalid"));
		isFieldValid = false;
	}

	FString ChartTitle;
	if (!Data->TryGetStringField(TEXT("chartTitle"), ChartTitle))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'ChartTitle' is missing or invalid"));
		isFieldValid = false;
	}

	FDataInstancePair DataPair;
	EJCMChartTypes CurChartTypeEnum = JCMDataTypes::JCMMapChartTypes[ChartType];

	switch (CurChartTypeEnum)
	{
	case None:
		break;
	case BAR:
	{
		// ������ ��ü ����
		UJCMDataContainerBar* NewChartBarClass = NewObject<UJCMDataContainerBar>();

		// X�� ������ ����
		const TSharedPtr<FJsonObject>* XAxisObject; 
		FString XName;
		TArray<FString> XLabels;
		const TArray<TSharedPtr<FJsonValue>>* LabelArray;

		if (!Data->TryGetObjectField(TEXT("xAxis"), XAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'xAxis' is missing or invalid"));
			isFieldValid = false;
		}
		else
		{
			// X�� �� �̸� ����
			if (!XAxisObject->Get()->TryGetStringField(TEXT("key"), XName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (x)' is missing or invalid"));
				isFieldValid = false;
			}

			// x�� ������ �迭 ����
			if (!XAxisObject->Get()->TryGetArrayField(TEXT("label"), LabelArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'label' is missing or invalid"));
				isFieldValid = false;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *LabelArray)
				{
					XLabels.Add(Value->AsString());
				}
			}
		}

		// Y�� ������ ����
		const TSharedPtr<FJsonObject>* YAxisObject;
		FString YName;
		TArray<float> YValues;
		const TArray<TSharedPtr<FJsonValue>>* ValueArray;

		if (!Data->TryGetObjectField(TEXT("yAxis"), YAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'yAxis' is missing or invalid"));
			isFieldValid = false;
		}
		else
		{
			// Y�� ������ �̸� ����
			if (!YAxisObject->Get()->TryGetStringField(TEXT("key"), YName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (y)' is missing or invalid"));
				isFieldValid = false;
			}
			// Y�� ������ �迭 ����
			if (!YAxisObject->Get()->TryGetArrayField(TEXT("value"), ValueArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'value' is missing or invalid"));
				isFieldValid = false;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *ValueArray)
				{
					YValues.Add(Value->AsNumber());
				}
			}
		}
		
		NewChartBarClass->SetChartData(ChartTitle, ChartType, XName, XLabels, YName, YValues);
		UE_LOG(JCMlog, Log, TEXT("%s"), *NewChartBarClass->GetChartDataStruct().ChartTitle);
		UE_LOG(JCMlog, Log, TEXT("%s"), *NewChartBarClass->GetChartDataStruct().ChartType);
		UE_LOG(JCMlog, Log, TEXT("%s"), *NewChartBarClass->GetChartDataStruct().XName);
		UE_LOG(JCMlog, Log, TEXT("%s"), *NewChartBarClass->GetChartDataStruct().YName);

		DataPair.IsValid = isFieldValid;
		DataPair.DataInstance = NewChartBarClass;
		return DataPair;
	}
		
	case LINE:
		break;
	case PIE:
		break;
	case XY:
		break;
	case XYZ:
		break;
	case FREE:
		break;
	default:
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Instancing Data Class was failed"));
	}
	return DataPair;
}

