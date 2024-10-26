// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManager.h"
#include "Datas/JsonHandler.h"
#include "Datas/CSVHandler.h"
#include "Serialization/JsonWriter.h"

// json ���� ���� ���� �а� ó���ϴ� ��ƾ
void UDataManager::LocalJsonReadProcessRoutine(const FString& FilePath)
{
	TSharedPtr<FJsonObject> Data = LoadDataFromJSON(FilePath);
	FDataInstancePair NewChartData = InstancingDataClass(Data);
	ChartDataClassInstanceArray.Add(NewChartData);
}

// 
void UDataManager::JsonObjectReadProcessRoutine(const TSharedPtr<FJsonObject> JsonData)
{
	if (JsonData.IsValid())
	{
		FDataInstancePair NewChartData = InstancingDataClass(JsonData);
		ChartDataClassInstanceArray.Add(NewChartData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DataManager: JsonObjectReadProcessRoutine : Json Data from HTTPRequestManager is invaild"));
	}

}

// FString���� Serialize�� Json���ڿ� ��ü�� �ٽ� ��ȯ
TSharedPtr<FJsonObject> UDataManager::DeserializeJsonStringToJsonObject(const FString& JsonString)
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
TSharedPtr<FJsonObject> UDataManager::LoadDataFromJSON(const FString& FilePath)
{
	if (!JSONHandlerInstance)
	{
		JSONHandlerInstance = NewObject<UJsonHandler>(this);
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

void UDataManager::LoadDataFromCSV(const FString& FilePath)
{
}

void UDataManager::FetchDataFromHTTP(const FString& URL)
{
}

// Return DataClass from ChartDataClassInstanceArray
UDataClasses* UDataManager::GetChartDataClassInstance(const FString& ClassName)
{
	UE_LOG(LogTemp, Log, TEXT("DataManager : %d"), ChartDataClassInstanceArray.Num());
	for (int32 i = 0; i < ChartDataClassInstanceArray.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("DataManager : Current Data Class Instance name : %s"), *ChartDataClassInstanceArray[i].ClassName)
		if (ChartDataClassInstanceArray[i].ClassName == ClassName)
		{
			UDataClasses* FindingReference = ChartDataClassInstanceArray[i].DataInstance;
			ChartDataClassInstanceArray.RemoveAt(i);
			return FindingReference;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : ChartDataClassInstanceQueue is empty or no such class"));
	return nullptr;
}

// Return JSON String Getter()
const FString& UDataManager::GetJSONStringData() const
{
	if (DataString == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : DataString is null"));
	}
	return DataString;
}

// JSON -> FString
FString UDataManager::SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject)
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
FDataInstancePair UDataManager::InstancingDataClass(const TSharedPtr<FJsonObject> Data)
{
	FString ChartType = Data->GetStringField(TEXT("chartType"));
	int32 ChartTypeNumber = DataTypes::MapChartTypes[ChartType.ToUpper()];
	EChartTypes CurChartTypeEnum = DataTypes::MapChartTypes[ChartType];
	FString ChartTitle = Data->GetStringField(TEXT("chartTitle"));

	FDataInstancePair DataPair;

	switch (CurChartTypeEnum)
	{
	case None:
		break;
	case BAR:
	{
		// ������ ��ü ����
		UShapeChartBarClass* NewChartBarClass = NewObject<UShapeChartBarClass>(this);

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


