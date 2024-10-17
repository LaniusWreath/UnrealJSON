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

// Struct With String Header And Data
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

class AAPITutorial;
// Manage Datas Class and Control those Member Function
UCLASS(Blueprintable, BlueprintType)
class TEST_API UDataManager : public UObject
{
	GENERATED_BODY()

private:

	AAPITutorial* GameModeInstance;

	// DataHander Instances
	UJsonHandler* JSONHandlerInstance;
	UCSVHandler* CSVHandlerInstance;
	UHTTPHandler* HTTPHandlerInstance;

	// JSON String Serialization Function
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);
	FString DataString;
	
	// JsonObject Ptr to Data Struct <String Header, UDataClasses* DataClassInstance>
	FDataInstancePair InstancingDataClass(TSharedPtr<FJsonObject>& Data);

	// Instancing JsonHandler and Get JsonObject Ptr
	TSharedPtr<FJsonObject> LoadDataFromJSON(const FString& FilePath);

	// Instancing CSVHandler and Get JsonObject Ptr
	void LoadDataFromCSV(const FString& FilePath);

	// Instancing HTTPHandler and Get JsonObject Ptr
	void FetchDataFromHTTP(const FString& URL);

	// Container for Data Struct
	UPROPERTY()
	TArray<FDataInstancePair> ChartDataClassInstanceArray;

public:

	UFUNCTION(BlueprintCallable, Category = "Data Management")
	void InitializeGameModeInstance();

	// Routine Function for Controlling Json Reading to Processing Functions
	UFUNCTION(BlueprintCallable, Category = "Data Management")
	void JsonReadProcessRoutine(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "DataManage")
	void JsonObjectReadProcessRoutine(int TargetIndex);

	// Getter Data Class Instance only without Header from Data Struct
	UFUNCTION(BlueprintCallable, Category = "Data Management")
	UDataClasses* GetChartDataClassInstance(const FString& ClassName);

	// Getter Serialized JSON String Data
	UFUNCTION(BlueprintCallable, Category = "Data")
	const FString& GetJSONStringData() const;
	
};

