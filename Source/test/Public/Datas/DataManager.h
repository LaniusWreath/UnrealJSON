// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataManager.generated.h"

class UJsonHandler;
class UCSVHandler;
class UHTTPHandler;

// ��� ��Ʈ�� ������ ����ü
USTRUCT(BlueprintType)
struct F2DShapeCharData
{
	GENERATED_BODY()

	// ��Ʈ Ÿ�� : "bar", "line", "pie" �� 
	UPROPERTY(BlueprintReadWrite)
	FString ChartType;   

	// x�� �� �Ǵ� pie ��Ʈ ����
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Labels;
	
	// y�� �� �Ǵ� pie ��Ʈ ����
	UPROPERTY(BlueprintReadWrite)
	TArray<float> Values;
};

USTRUCT(BlueprintType)
struct F2DXYChartData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<float> XData;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> YData;
};

USTRUCT(BlueprintType)
struct F3DXYZChartData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Coordinates;
};

UCLASS(Blueprintable, BlueprintType)
class TEST_API UDataManager : public UObject
{
	GENERATED_BODY()

public:
	
	// DataHander �ν��Ͻ� ����
	UFUNCTION(BlueprintCallable, Category = "Data Management") // JSON
	void LoadDataFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // CSV
	void LoadDataFromCSV(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // HTTP
	void FetchDataFromHTTP(const FString& URL);


	// 2D ������ ��Ʈ�� ������ ����
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F2DShapeCharData ShapeChartData;

	// 2D XY ��Ʈ�� ������ ����
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F2DXYChartData XYChartData;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F3DXYZChartData XYZChartData;

	// ������ JSON
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	TMap<FString, FString> FreeFormData;

	// ���� const : ��ȯ�� ���۷����� ���, ���� const : �� �Լ��� ��ü�� ���¸� �������� ����
	UFUNCTION(BlueprintCallable, Category = "Data Manager")
	const TMap<FString, FString>& GetJSONParsedData() const;


private:

	// Data Load Object References
	UPROPERTY()
	UJsonHandler* JSONHandler;

	UPROPERTY()
	UCSVHandler* CSVHandler;

	UPROPERTY()
	UHTTPHandler* HTTPHandler;
	
};
