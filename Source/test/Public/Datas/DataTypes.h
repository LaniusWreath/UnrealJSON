// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes.generated.h"

// 모양 차트용 데이터 구조체
USTRUCT(BlueprintType)
struct FShapeChartData
{
	GENERATED_BODY()

	// 차트 타입 : "bar", "line", "pie" 등 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ChartType = "bar";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString XName = "Date";
	// x축 라벨 또는 pie 차트 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Labels = {"2024-01-01", "2024-01-02", "2024-01-03", "2024-01-04"};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString YName = "Temperature";
	// y축 값 또는 pie 차트 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Values = { 20.5, 21.0, 19.8, 22.1 };


};

USTRUCT(BlueprintType)
struct FXYChartData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString XName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> XData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString YName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> YData;
};

USTRUCT(BlueprintType)
struct FXYZChartData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Names;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Coordinates;
};

// 열거형
UENUM(BlueprintType)
enum EChartTypes : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	E_SHAPE = 1	UMETA(DisplayName = "Shape"),
	E_XY = 2 UMETA(DisplayName = "XY"),
	E_XYZ = 3 UMETA(DisplayName = "XYZ"),
	E_FREE = 4 UMETA(DisplayName = "FREE"),

};

class TEST_API DataTypes
{
public:
	DataTypes();
	~DataTypes();
};

