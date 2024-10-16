// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes.generated.h"

// DataClasses Child Class Names for SetDataClassInstance()
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ChartTitle;

	// Chart Types : "bar", "line", "pie" .. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ChartType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString XName;

	// Label for X Axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> Labels;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString YName;
	
	// Value for Y Axis or Quotiont for PieChart
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> Values;
};

USTRUCT(BlueprintType)
struct FXYChartData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ChartTitle;

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
	FString ChartTitle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> Names;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Coordinates;
};

// ¿­°ÅÇü
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


