// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes.generated.h"

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

class TEST_API DataTypes
{
public:
	DataTypes();
	~DataTypes();
};

