// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes.h"
#include "DataClasses.generated.h"

// Mapping Chart Data and Returning Instance Factory Method : Abstract
UCLASS(Abstract)
class TEST_API UDataClasses : public UObject
{
	GENERATED_BODY()

private:

protected:
	EChartTypes ChartType = EChartTypes::None;

public:
	UFUNCTION()
	virtual const EChartTypes GetChartType() const;
};

// �� Ŭ������ ��ü�� DataManager���� �����ǰ� ���� ��. 
// ó���� ������ ������ DataTypes�� ������ ���󰡸�, �����Ͱ� �ƴ� ���� ���� ��� ������ ��������.
// ó���� ������ ������ �ʿ��ϴٸ� �� �ڼ� Ŭ������ Getter �Լ��� ����� ��.
/////////////////////////////////////////////////////////////////////////////////////////////////

// Mapping Shape Chart Data 
UCLASS(BlueprintType)
class UShapeChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FShapeChartData ShapeChartData;

public:
	// Getting Data Struct Member for Mapping
	virtual bool SetChartData(const FString& ChartTitle, const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FShapeChartData& InputData);

	// Getter Data Struct Reference
	UFUNCTION(BlueprintCallable, Category = "Chart")
	const FShapeChartData& GetShapeChartData() const
	{
		return ShapeChartData;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(BlueprintType)
class UShapeChartBarClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::BAR;

public:
	virtual bool SetChartData(const FShapeChartData& InputData) override;
	virtual bool SetChartData(const FString& ChartTitle, const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels,
		const FString& YName, const TArray<float>& Values) override;

	// Getter Enum Chart Type
	virtual const EChartTypes GetChartType() const override
	{
		return ChartType;
	}
};

UCLASS()
class UShapeChartLineClass : public UShapeChartClass
{
	GENERATED_BODY()
	

private:
	EChartTypes ChartType = EChartTypes::LINE;

public:

};

UCLASS()
class UShapeChartPieClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::PIE;

public:

};

//////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class UXYChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::XY;
	FXYChartData XYChartData;

public:

	virtual bool SetChartData(const FString& XName, const TArray<float>& XData, const FString& YName, const TArray<float>& YData);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FXYChartData& InputData);
};

UCLASS()
class UXYZChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::XYZ;
	FXYZChartData XYZChartData;

public:

	virtual bool SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FXYZChartData& InputData);
};
