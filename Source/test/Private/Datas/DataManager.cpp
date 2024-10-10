// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManager.h"
#include "Datas/JsonHandler.h"
#include "Datas/CSVHandler.h"
#include "Datas/HTTPHandler.h"
#include "Serialization/JsonWriter.h"

void UDataManager::JsonReadProcessRoutine(const FString& FilePath)
{
	TSharedPtr<FJsonObject> Data = LoadDataFromJSON(FilePath);
	FDataInstancePair NewChartData = InstancingDataClass(Data);
	ChartDataClassInstanceArray.Add(NewChartData);
}

// 탐색기 패스로 파일 읽기, 나중에 오버로딩 하던 뭘 하던 JSON만 넘겨받는 함수 만들 것
TSharedPtr<FJsonObject> UDataManager::LoadDataFromJSON(const FString& FilePath)
{
	if (!JSONHandlerInstance)
	{
		JSONHandlerInstance = NewObject<UJsonHandler>(this);
	}
	if (JSONHandlerInstance)
	{
		// JSONHandler에서 읽은 JSON 포인터 그대로 가져옴, 이거 받는 함수만 따로 분리
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

// 데이터 입력 받아 DataClass 객체 생성 -> Chart
FDataInstancePair UDataManager::InstancingDataClass(TSharedPtr<FJsonObject>& Data)
{
	FString ChartType = Data->GetStringField(TEXT("chartType"));
	int32 ChartTypeNumber = DataTypes::MapChartTypes[ChartType.ToUpper()];
	EChartTypes CurChartTypeEnum = DataTypes::MapChartTypes[ChartType];

	switch (CurChartTypeEnum)
	{
	case None:
		break;
	case BAR:
	{
		// 데이터 객체 생성
		UShapeChartBarClass* NewChartBarClass = NewObject<UShapeChartBarClass>(this);

		FString ClassName = NewChartBarClass->GetClass()->GetName();

		// X축 라벨 이름 추출
		const TSharedPtr<FJsonObject> XAxisObject = Data->GetObjectField(TEXT("xAxis"));
		FString XName = XAxisObject->GetStringField(TEXT("key"));

		// x축 데이터 배열 추출
		TArray<FString> XLabels;
		TArray<TSharedPtr<FJsonValue>> LabelArray = XAxisObject->GetArrayField(TEXT("data"));
		for (const TSharedPtr<FJsonValue>& Value : LabelArray)
		{
			XLabels.Add(Value->AsString());
		}

		// Y축 라벨 이름 추출
		const TSharedPtr<FJsonObject> YAxisObject = Data->GetObjectField(TEXT("yAxis"));
		FString YName = YAxisObject->GetStringField(TEXT("key"));

		// Y축 데이터 배열 추출
		TArray<float> YValues;
		TArray<TSharedPtr<FJsonValue>> ValueArray = YAxisObject->GetArrayField(TEXT("data"));
		for (const TSharedPtr<FJsonValue>& Value : ValueArray)
		{
			YValues.Add(Value->AsNumber());
		}
		NewChartBarClass->SetChartData(ChartType, XName, XLabels, YName, YValues);

		FDataInstancePair DataPair(ClassName, NewChartBarClass);
		
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
	}
	UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : Instancing Data Class was failed"));
	return {TEXT("None") ,nullptr};
}


