// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataClasses.h"
#include "DataManager.generated.h"

class UJsonHandler;
class UCSVHandler;
class UHTTPHandler;

UCLASS(Blueprintable, BlueprintType)
class TEST_API UDataManager : public UObject
{
	GENERATED_BODY()

private:

	// Property
	UJsonHandler* JSONHandlerInstance;
	UCSVHandler* CSVHandlerInstance;
	UHTTPHandler* HTTPHandlerInstance;

	// JSON String 직렬화
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);

	FString DataString;
	
	// Function
	void InstancingDataClass(TSharedPtr<FJsonObject>& Data);


public:
	
	// DataHander 인스턴스 저장
	UFUNCTION(BlueprintCallable, Category = "Data Management") // JSON
	void LoadDataFromJSON(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // CSV
	void LoadDataFromCSV(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // HTTP
	void FetchDataFromHTTP(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // 데이터 클래스 큐에서 앞에것 팝
	UDataClasses* GetLastChartDataClassInstancePtr();

	// 셰이프 차트용 데이터 저장
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UDataClasses*> ChartDataClassInstanceQueue;

	// 2D XY 차트용 데이터 저장
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FXYChartData XYChartData;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FXYZChartData XYZChartData;

	// 자유형 JSON
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TMap<FString, FString> FreeFormData;

	// 차트 유형 열거형
	EChartTypes LastChartType = EChartTypes::None;

	// DataManager에서 전체 JSON 받아갈 수 있게끔 JSONHandler 참조 연결
	// 앞쪽 const : 반환된 레퍼런스가 상수, 끝쪽 const : 이 함수가 객체의 상태를 변경하지 않음
	UFUNCTION(BlueprintCallable, Category = "Data")
	const FString& GetJSONStringData() const;
	
};
