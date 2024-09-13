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
struct FShapeChartData
{
	GENERATED_BODY()

	// ��Ʈ Ÿ�� : "bar", "line", "pie" �� 
	UPROPERTY(BlueprintReadOnly)
	FString ChartType;   

	UPROPERTY(BlueprintReadOnly)
	FString XName;
	// x�� �� �Ǵ� pie ��Ʈ ����
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Labels;
	
	UPROPERTY(BlueprintReadOnly)
	FString YName;
	// y�� �� �Ǵ� pie ��Ʈ ����
	UPROPERTY(BlueprintReadOnly)
	TArray<float> Values;
};

USTRUCT(BlueprintType)
struct FXYChartData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString XName;
	UPROPERTY(BlueprintReadOnly)
	TArray<float> XData;

	UPROPERTY(BlueprintReadOnly)
	FString YName;
	UPROPERTY(BlueprintReadOnly)
	TArray<float> YData;
};

USTRUCT(BlueprintType)
struct FXYZChartData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Names;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> Coordinates;
};

// ������
UENUM(BlueprintType)
enum EChartTypes : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	E_SHAPE = 1	UMETA(DisplayName = "Shape"),
	E_XY = 2 UMETA(DisplayName = "XY"),
	E_XYZ = 3 UMETA(DisplayName = "XYZ"),
	E_FREE = 4 UMETA(DisplayName = "FREE"),

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
