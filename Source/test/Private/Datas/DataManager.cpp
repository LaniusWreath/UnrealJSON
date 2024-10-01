// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManager.h"
#include "Datas/JsonHandler.h"
#include "Datas/CSVHandler.h"
#include "Datas/HTTPHandler.h"
#include "Serialization/JsonWriter.h"


// 탐색기 패스로 파일 읽기, 나중에 오버로딩 하던 뭘 하던 JSON만 넘겨받는 함수 만들 것
void UDataManager::LoadDataFromJSON(const FString& FilePath)
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
	}

		//	// 열거형에 차트타입 저장
		//	int32 ChartTypeNumber = DataTypes::MapChartTypes[ChartType.ToUpper()];
		//	UE_LOG(LogTemp, Log, TEXT("DataManger : CurrentChartTypeNumber is %d"), ChartTypeNumber);
		//	switch (ChartTypeNumber)
		//	{
		//	case(1):
		//	{
		//		LastChartType = EChartTypes::BAR;
		//		break;
		//	}
		//	case(2):
		//	{
		//		LastChartType = EChartTypes::LINE;
		//		break;
		//	}
		//	case(3):
		//	{
		//		LastChartType = EChartTypes::PIE;
		//		break;
		//	}
		//	default:
		//		break;
		//	}
		//	
		//}

		// 차트 타입이 2차원 xy일 경우
		//else if (ChartType == "xy")
		//{
		//	// x축 데이터
		//	const TSharedPtr<FJsonObject> XAxisObject = ParsedData->GetObjectField(TEXT("xAxis"));

		//	XYChartData.XName = XAxisObject->GetStringField(TEXT("key"));

		//	TArray<TSharedPtr<FJsonValue>> XArray = XAxisObject->GetArrayField(TEXT("data"));
		//	XYChartData.XData.Empty();
		//	for (const TSharedPtr<FJsonValue>& Value : XArray)
		//	{
		//		XYChartData.XData.Add(Value->AsNumber());
		//	}

		//	// y축 데이터
		//	const TSharedPtr<FJsonObject> YAxisObject = ParsedData->GetObjectField(TEXT("yAxis"));

		//	XYChartData.YName = YAxisObject->GetStringField(TEXT("key"));

		//	TArray<TSharedPtr<FJsonValue>> YArray = YAxisObject->GetArrayField(TEXT("data"));
		//	XYChartData.YData.Empty();
		//	for (const TSharedPtr<FJsonValue>& Value : YArray)
		//	{
		//		XYChartData.YData.Add(Value->AsNumber());
		//	}

		//	LastChartType = EChartTypes::XY;
		//}

		//// 차트 타입이 3차원 벡터일 경우
		//else if (ChartType == "xyz")
		//{
		//	const TArray<TSharedPtr<FJsonValue>> ParsedCoordinates = ParsedData->GetArrayField(TEXT("coordinates"));
		//	XYZChartData.Coordinates.Empty();

		//	for (const TSharedPtr<FJsonValue>& Value : ParsedCoordinates)
		//	{
		//		const TSharedPtr<FJsonObject> CoordinateObject = Value->AsObject();
		//		FString PointName = CoordinateObject->GetStringField(TEXT("label"));
		//		float X = CoordinateObject->GetNumberField(TEXT("x"));
		//		float Y = CoordinateObject->GetNumberField(TEXT("y"));
		//		float Z = CoordinateObject->GetNumberField((TEXT("z")));

		//		FVector Coordinate(X, Y, Z);
		//		XYZChartData.Coordinates.Add(Coordinate);
		//		XYZChartData.Names.Add(PointName);
		//	}

		//	LastChartType = EChartTypes::XYZ;
		//}
		//
		//// 차트 타입이 자유로운 형식일 경우
		//else if (ChartType == "free")
		//{
		//	FreeFormData.Empty();
		//	for (const auto& Pair : ParsedData->Values)
		//	{
		//		// JSON 값이 문자열인 경우만 처리(나중에 필요한 데이터 계속 추가할 것, boolean 등)
		//		if (Pair.Value->Type == EJson::String)
		//		{
		//			FreeFormData.Add(Pair.Key, Pair.Value->AsString());
		//		}
		//		else if (Pair.Value -> Type == EJson::Number)
		//		{
		//			FreeFormData.Add(Pair.Key, FString::SanitizeFloat(Pair.Value->AsNumber()));
		//		}
		//		else
		//		{
		//			FreeFormData.Add(Pair.Key, TEXT("Unsupported data Type"));
		//		}
		//	}
		//	
		//	LastChartType = EChartTypes::FREE;
		//}

		//// 차트 타입 에러
		//else
		//{
		//	UE_LOG(LogTemp, Error, TEXT("Unvaild Chart Type : %s" ),*ChartType );
		//}

	//UE_LOG(LogTemp, Log, TEXT("Chart Data Parsed for Type : %s"), *ChartType);
}

void UDataManager::LoadDataFromCSV(const FString& FilePath)
{
}

void UDataManager::FetchDataFromHTTP(const FString& URL)
{
}

UDataClasses* UDataManager::GetLastChartDataClassInstancePtr()
{
	if (ChartDataClassInstanceQueue.Num() > 0)
	{
		UDataClasses* ptr = ChartDataClassInstanceQueue[0];
		ChartDataClassInstanceQueue.RemoveAt(0);
		return ptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DataManager.cpp : ChartDataClassInstanceQueue is empty"));
		return nullptr;
	}
}

// FString 으로 변환한 JSON Getter()
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

void UDataManager::InstancingDataClass(TSharedPtr<FJsonObject>& Data)
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
		ChartDataClassInstanceQueue.Add(NewChartBarClass);

		break;
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
}


