// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes.generated.h"


namespace ChartClassNames
{
	static const FString NAME_BARCHART = TEXT("ShapeChartBarClass");
	static const FString NAME_LINECHART = TEXT("ShapeChartLineClass");
	static const FString NAME_PIECHART = TEXT("ShapeChartPieClass");
	static const FString NAME_XYCHART = TEXT("XYChartClass");
	static const FString NAME_XYZCHART = TEXT("XYZChartClass");
}


// Struct for Shape Chart Data
USTRUCT(BlueprintType)
struct FShapeChartData
{
	GENERATED_BODY()

	// 차트 타입 : "bar", "line", "pie" 등 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ChartType = "bar";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString XName = "Date";
	// x축 라벨 또는 pie 차트 레벨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> Labels = {"2024-01-01", "2024-01-02", "2024-01-03", "2024-01-04"};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString YName = "Temperature";
	// y축 값 또는 pie 차트 비율
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> Values = { 20.5, 21.0, 19.8, 22.1 };
};

USTRUCT(BlueprintType)
struct FXYChartData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString XName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> XData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString YName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> YData;
};

USTRUCT(BlueprintType)
struct FXYZChartData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> Names;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Coordinates;
};

// 열거형
UENUM(BlueprintType)
enum EChartTypes : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	BAR = 1 UMETA(DisplayName = "BAR"),
	LINE = 2 UMETA(DisplayName = "LINE"),
	PIE = 3 UMETA(DisplayName = "PIE"),
	XY = 4 UMETA(DisplayName = "XY"),
	XYZ = 5 UMETA(DisplayName = "XYZ"),
	FREE = 6 UMETA(DisplayName = "FREE"),

};

class TEST_API DataTypes
{
public:
	DataTypes();

	UPROPERTY(VisibleAnywhere)
	static TMap<FString, EChartTypes> MapChartTypes ;

	~DataTypes();
};


