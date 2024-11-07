// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JCMDataManager.h"
#include "Datas/JCMJsonHandler.h"
#include "Datas/CSVHandler.h"
#include "Datas/JCMDataContainer.h"
#include "Serialization/JsonWriter.h"

// json ���� ���� ���� �а� ó���ϴ� ��ƾ
UJCMDataContainer* UJCMDataManager::InstancingDataContainerFromLocalJson(const FString& FilePath)
{
	TSharedPtr<FJsonObject> Data = LoadDataFromJSON(FilePath);
	FDataInstancePair NewChartData = InstancingDataClass(Data);
	return NewChartData.DataInstance;
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
		UE_LOG(LogTemp, Log, TEXT("Successfully deserialized JSON string to FJsonObject."));
		return JsonObject;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON string."));
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
		UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : DataString is null"));
	}
	return DataString;
}

// Create Emtpy BarType Data Container Instance
UJCMDataContainerBar* UJCMDataManager::CreateEmptyShapeChartDataInstance()
{
	return NewObject<UJCMDataContainerBar>();
}

// JSON -> FString
FString UJCMDataManager::SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);

	if (FJsonSerializer::Serialize(JSONObject.ToSharedRef(), Writer))
	{
		UE_LOG(LogTemp, Log, TEXT("DataManager: JSONToString Serialized"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("DataManager: JSONToString Serialize Failed"));
	}

	return JsonString;
}

// ������ �Է� �޾� �Ľ�, DataClass ��ü ���� -> Chart
FDataInstancePair UJCMDataManager::InstancingDataClass(const TSharedPtr<FJsonObject> Data)
{
	if (!Data.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("DataManager : InstancingDataClass : Input Data is invalid"));
		return FDataInstancePair();
	}

	FString ChartType = Data->GetStringField(TEXT("chartType"));
	int32 ChartTypeNumber = JCMDataTypes::JCMMapChartTypes[ChartType.ToUpper()];
	EJCMChartTypes CurChartTypeEnum = JCMDataTypes::JCMMapChartTypes[ChartType];
	FString ChartTitle = Data->GetStringField(TEXT("chartTitle"));

	FDataInstancePair DataPair;

	switch (CurChartTypeEnum)
	{
	case None:
		break;
	case BAR:
	{
		// ������ ��ü ����
		UJCMDataContainerBar* NewChartBarClass = NewObject<UJCMDataContainerBar>(this);

		FString ClassName = NewChartBarClass->GetClass()->GetName();

		// X�� �� �̸� ����
		const TSharedPtr<FJsonObject> XAxisObject = Data->GetObjectField(TEXT("xAxis"));
		FString XName = XAxisObject->GetStringField(TEXT("key"));

		// x�� ������ �迭 ����
		TArray<FString> XLabels;
		TArray<TSharedPtr<FJsonValue>> LabelArray = XAxisObject->GetArrayField(TEXT("data"));
		for (const TSharedPtr<FJsonValue>& Value : LabelArray)
		{
			XLabels.Add(Value->AsString());
		}

		// Y�� �� �̸� ����
		const TSharedPtr<FJsonObject> YAxisObject = Data->GetObjectField(TEXT("yAxis"));
		FString YName = YAxisObject->GetStringField(TEXT("key"));

		// Y�� ������ �迭 ����
		TArray<float> YValues;
		TArray<TSharedPtr<FJsonValue>> ValueArray = YAxisObject->GetArrayField(TEXT("data"));
		for (const TSharedPtr<FJsonValue>& Value : ValueArray)
		{
			YValues.Add(Value->AsNumber());
		}
		NewChartBarClass->SetChartData(ChartTitle, ChartType, XName, XLabels, YName, YValues);

		DataPair.ClassName = ClassName;
		DataPair.DataInstance = NewChartBarClass;

		UE_LOG(LogTemp, Log, TEXT("DataManager : DataClass Instanced"));
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
		UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : Instancing Data Class was failed"));
	}
	return DataPair;
}

