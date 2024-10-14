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
	virtual void ProcessData() {}
	EChartTypes ChartType = EChartTypes::None;

public:
	virtual const EChartTypes GetChartType() const;
};

// �� Ŭ������ ��ü�� DataManager���� �����ǰ� ���� ��. 
// ó���� ������ ������ DataTypes�� ������ ���󰡸�, �����Ͱ� �ƴ� ���� ���� ��� ������ ��������.
// ó���� ������ ������ �ʿ��ϴٸ� �� �ڼ� Ŭ������ Getter �Լ��� ����� ��.
/////////////////////////////////////////////////////////////////////////////////////////////////

// Mapping Shape Chart Data 
UCLASS()
class UShapeChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FShapeChartData ShapeChartData;

	virtual void ProcessData() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing Shape Chart Data"));
	}

public:
	// Getting Data Struct Member for Mapping
	virtual bool SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FShapeChartData& InputData);

	// Getter Only Values
	virtual const TArray<float>& GetShapeChartDataValues() const;

	// Getter Data Struct Reference
	const FShapeChartData& GetShapeChartData() const
	{
		return ShapeChartData;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class UShapeChartBarClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::BAR;

	void ProcessData() override {};

public:
	virtual bool SetChartData(const FShapeChartData& InputData) override;
	virtual bool SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, 
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

	void ProcessData() override {};

public:

};

UCLASS()
class UShapeChartPieClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::PIE;

	void ProcessData() override {};

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

	virtual void ProcessData() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing XY Chart Data"));
	}

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

	virtual void ProcessData() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing XYZ Chart Data"));
	}

public:

	virtual bool SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FXYZChartData& InputData);
};