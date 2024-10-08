// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataClasses.h"
#include "Templates/SharedPointer.h"
#include "DataManager.generated.h"

class UJsonHandler;
class UCSVHandler;
class UHTTPHandler;

USTRUCT(BlueprintType)
struct FDataInstancePair
{
	GENERATED_BODY()

	UPROPERTY()
	FString ClassName;

	UPROPERTY()
	UDataClasses* DataInstance;

	FDataInstancePair() : ClassName(TEXT("")), DataInstance(nullptr) {}
	FDataInstancePair(const FString& InClassName, UDataClasses* InInstance)
		: ClassName(InClassName), DataInstance(InInstance) {}
};


UCLASS(Blueprintable, BlueprintType)
class TEST_API UDataManager : public UObject
{
	GENERATED_BODY()

private:

	// DataHander 인스턴스 저장
	UJsonHandler* JSONHandlerInstance;
	UCSVHandler* CSVHandlerInstance;
	UHTTPHandler* HTTPHandlerInstance;

	// JSON String 직렬화
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);
	FString DataString;
	
	// Function
	FDataInstancePair InstancingDataClass(TSharedPtr<FJsonObject>& Data);

	// JSON 데이터 가져와서 포인터 객체로 반환
	TSharedPtr<FJsonObject> LoadDataFromJSON(const FString& FilePath);

	// CSV
	void LoadDataFromCSV(const FString& FilePath);

	// HTTP
	void FetchDataFromHTTP(const FString& URL);

	// 셰이프 차트용 데이터 저장 컨테이너
	UPROPERTY()
	TArray<FDataInstancePair> ChartDataClassInstanceArray;

public:

	UFUNCTION(BlueprintCallable, Category = "Data Management")
	void JsonReadProcessRoutine(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // 데이터 클래스 큐에서 앞에것 팝
	UDataClasses* GetChartDataClassInstance(const FString& ClassName);

	// 특정 차트 데이터만 찾아서 가져오는 함수도 만들어야

	// DataManager에서 전체 JSON 받아갈 수 있게끔 JSONHandler 참조 연결
	// 앞쪽 const : 반환된 레퍼런스가 상수, 끝쪽 const : 이 함수가 객체의 상태를 변경하지 않음
	UFUNCTION(BlueprintCallable, Category = "Data")
	const FString& GetJSONStringData() const;
	
};

