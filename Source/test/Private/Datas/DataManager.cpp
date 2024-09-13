// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManager.h"
#include "Datas/JsonHandler.h"
#include "Datas/CSVHandler.h"
#include "Datas/HTTPHandler.h"

void UDataManager::LoadDataFromJSON(const FString& FilePath)
{
	if (!JSONHandler)
	{
		JSONHandler = NewObject<UJsonHandler>(this);
	}
	if (JSONHandler)
	{
		// JSONHandler���� ���� JSON ��ü ȣ��
		const TSharedPtr<FJsonObject> ParsedData = JSONHandler->GetJsonObjectData(FilePath);

		// ��Ʈ ���� Ȯ��, ��Ʈ ������ ���߿� enum���� �����ص� ��
		FString ChartType = ParsedData->GetStringField(TEXT("chartType"));

		// ��Ʈ�� ���� ��Ʈ�� ���
		if (ChartType == "bar" || ChartType == "line" || ChartType == "pie")
		{
			ShapeChartData.ChartType = ChartType;

			// x�� �� or ���� ��Ʈ ��, ������ �ɰ� ����
			const TSharedPtr<FJsonObject> XAxisObject = ParsedData->GetObjectField(TEXT("xAxis"));
			TArray<TSharedPtr<FJsonValue>> LabelArray = XAxisObject->GetArrayField(TEXT("data"));
			ShapeChartData.Labels.Empty();
			for (const TSharedPtr<FJsonValue>& Value : LabelArray)
			{
				ShapeChartData.Labels.Add(Value->AsString());
			}

			// y�� �� �Ǵ� ���� ��Ʈ ��
			const TSharedPtr<FJsonObject> YAxisObject = ParsedData->GetObjectField(TEXT("yAxis"));
			TArray<TSharedPtr<FJsonValue>> ValueArray = YAxisObject->GetArrayField("data");
			ShapeChartData.Values.Empty();
			for (const TSharedPtr<FJsonValue>& Value : ValueArray)
			{
				ShapeChartData.Values.Add(Value->AsNumber());
			}
		}

		// ��Ʈ Ÿ���� 2���� xy�� ���
		else if (ChartType == "xy")
		{
			// x�� ������
			const TSharedPtr<FJsonObject> XAxisObject = ParsedData->GetObjectField(TEXT("xAxis"));
			TArray<TSharedPtr<FJsonValue>> XArray = XAxisObject->GetArrayField(TEXT("data"));
			XYChartData.XData.Empty();
			for (const TSharedPtr<FJsonValue>& Value : XArray)
			{
				XYChartData.XData.Add(Value->AsNumber());
			}

			// y�� ������
			const TSharedPtr<FJsonObject> YAxisObject = ParsedData->GetObjectField(TEXT("yAxis"));
			TArray<TSharedPtr<FJsonValue>> YArray = YAxisObject->GetArrayField(TEXT("data"));
			XYChartData.YData.Empty();
			for (const TSharedPtr<FJsonValue>& Value : YArray)
			{
				XYChartData.YData.Add(Value->AsNumber());
			}
		}

		// ��Ʈ Ÿ���� 3���� ������ ���
		else if (ChartType == "xyz")
		{
			const TArray<TSharedPtr<FJsonValue>> ParsedCoordinates = ParsedData->GetArrayField(TEXT("coordinates"));
			XYZChartData.Coordinates.Empty();

			for (const TSharedPtr<FJsonValue>& Value : ParsedCoordinates)
			{
				const TSharedPtr<FJsonObject> CoordinateObject = Value->AsObject();
				float X = CoordinateObject->GetNumberField(TEXT("x"));
				float Y = CoordinateObject->GetNumberField(TEXT("y"));
				float Z = CoordinateObject->GetNumberField((TEXT("z")));

				FVector Coordinate(X, Y, Z);
				XYZChartData.Coordinates.Add(Coordinate);
			}
		}
		
		//��Ʈ Ÿ���� �����ο� ������ ���
		else if (ChartType == "free")
		{
			FreeFormData.Empty();
			for (const auto& Pair : ParsedData->Values)
			{
				// JSON ���� ���ڿ��� ��츸 ó��(���߿� �ʿ��� ������ ��� �߰��� ��, boolean ��)
				if (Pair.Value->Type == EJson::String)
				{
					FreeFormData.Add(Pair.Key, Pair.Value->AsString());
				}
				else if (Pair.Value -> Type == EJson::Number)
				{
					FreeFormData.Add(Pair.Key, FString::SanitizeFloat(Pair.Value->AsNumber()));
				}
				else
				{
					FreeFormData.Add(Pair.Key, TEXT("Unsupported data Type"));
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Chart Data Parsed for Type : %s"), *ChartType);
	}
}

void UDataManager::LoadDataFromCSV(const FString& FilePath)
{
}

void UDataManager::FetchDataFromHTTP(const FString& URL)
{
}

const TMap<FString, FString>& UDataManager::GetJSONParsedData() const
{

}

