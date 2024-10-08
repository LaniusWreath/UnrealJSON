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

	// DataHander �ν��Ͻ� ����
	UJsonHandler* JSONHandlerInstance;
	UCSVHandler* CSVHandlerInstance;
	UHTTPHandler* HTTPHandlerInstance;

	// JSON String ����ȭ
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);
	FString DataString;
	
	// Function
	FDataInstancePair InstancingDataClass(TSharedPtr<FJsonObject>& Data);

	// JSON ������ �����ͼ� ������ ��ü�� ��ȯ
	TSharedPtr<FJsonObject> LoadDataFromJSON(const FString& FilePath);

	// CSV
	void LoadDataFromCSV(const FString& FilePath);

	// HTTP
	void FetchDataFromHTTP(const FString& URL);

	// ������ ��Ʈ�� ������ ���� �����̳�
	UPROPERTY()
	TArray<FDataInstancePair> ChartDataClassInstanceArray;

public:

	UFUNCTION(BlueprintCallable, Category = "Data Management")
	void JsonReadProcessRoutine(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Data Management") // ������ Ŭ���� ť���� �տ��� ��
	UDataClasses* GetChartDataClassInstance(const FString& ClassName);

	// Ư�� ��Ʈ �����͸� ã�Ƽ� �������� �Լ��� ������

	// DataManager���� ��ü JSON �޾ư� �� �ְԲ� JSONHandler ���� ����
	// ���� const : ��ȯ�� ���۷����� ���, ���� const : �� �Լ��� ��ü�� ���¸� �������� ����
	UFUNCTION(BlueprintCallable, Category = "Data")
	const FString& GetJSONStringData() const;
	
};

