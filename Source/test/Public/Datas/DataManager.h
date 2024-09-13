// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataManager.generated.h"

class UJsonHandler;
class UCSVHandler;
class UHTTPHandler;

// 모양 차트용 데이터 구조체
USTRUCT(BlueprintType)
struct F2DShapeCharData
{
	GENERATED_BODY()

	// 차트 타입 : "bar", "line", "pie" 등 
	UPROPERTY(BlueprintReadWrite)
	FString ChartType;   

	// x축 라벨 또는 pie 차트 레벨
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Labels;
	
	// y축 값 또는 pie 차트 비율
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
	
	// DataHander 인스턴스 저장
	UFUNCTION(BlueprintCallable, Category = "Data Management") // JSON
	void LoadDataFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // CSV
	void LoadDataFromCSV(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // HTTP
	void FetchDataFromHTTP(const FString& URL);


	// 2D 셰이프 차트용 데이터 저장
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F2DShapeCharData ShapeChartData;

	// 2D XY 차트용 데이터 저장
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F2DXYChartData XYChartData;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	F3DXYZChartData XYZChartData;

	// 자유형 JSON
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	TMap<FString, FString> FreeFormData;

	// 앞쪽 const : 반환된 레퍼런스가 상수, 끝쪽 const : 이 함수가 객체의 상태를 변경하지 않음
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
