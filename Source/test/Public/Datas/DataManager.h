// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes.h"
#include "DataManager.generated.h"

class UJsonHandler;
class UCSVHandler;
class UHTTPHandler;


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


	// ������ ��Ʈ�� ������ ����
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FShapeChartData ShapeChartData;

	// 2D XY ��Ʈ�� ������ ����
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FXYChartData XYChartData;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FXYZChartData XYZChartData;

	// ������ JSON
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TMap<FString, FString> FreeFormData;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString JSONString;

	// ��Ʈ ���� ������
	EChartTypes LastChartType = EChartTypes::None;

	// DataManager���� ��ü JSON �޾ư� �� �ְԲ� JSONHandler ���� ����
	// ���� const : ��ȯ�� ���۷����� ���, ���� const : �� �Լ��� ��ü�� ���¸� �������� ����
	UFUNCTION(BlueprintCallable, Category = "Data")
	const FString& GetJSONStringData() const;


private:

	// Data Load Object References
	UPROPERTY()
	UJsonHandler* JSONHandlerInstance;

	UPROPERTY()
	UCSVHandler* CSVHandlerInstance;

	UPROPERTY()
	UHTTPHandler* HTTPHandlerInstance;

	// JSON String ����ȭ
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);
	
};
