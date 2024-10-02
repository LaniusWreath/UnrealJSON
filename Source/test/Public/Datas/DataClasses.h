// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTypes.h"
#include "DataClasses.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UDataClasses : public UObject
{
	GENERATED_BODY()

private:
	EChartTypes ChartType = EChartTypes::None;

protected:
	virtual void ProcessData() {}
	virtual void Initialize(EChartTypes Type) { ChartType = Type; }

public:
	virtual const EChartTypes GetChartType() const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

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
	virtual bool SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FShapeChartData& InputData);

	virtual const TArray<float>& GetBarChartDataValues() const;

	// 구조체에 대한 참조 반환, 상수.
	const FShapeChartData& GetShapeChartData() const
	{
		return ShapeChartData;
	}
};

UCLASS()
class UShapeChartBarClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	void ProcessData() override {};

public:
	// 이 형태로 다른 자식 클래스도 함수 오버라이딩 수정할것
	virtual void Initialize(EChartTypes Type) override { Super::Initialize(EChartTypes::BAR); }
	virtual bool SetChartData(const FShapeChartData& InputData) override;
	virtual bool SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, 
		const FString& YName, const TArray<float>& Values) override;
};

UCLASS()
class UShapeChartLineClass : public UShapeChartClass
{
	GENERATED_BODY()
	

private:
	void ProcessData() override {};

public:
	virtual void Initialize(EChartTypes Type) override { Super::Initialize(EChartTypes::LINE); }
};

UCLASS()
class UShapeChartPieClass : public UShapeChartClass
{
	GENERATED_BODY()

private:
	void ProcessData() override {};

public:
	virtual void Initialize(EChartTypes Type) override { Super::Initialize(EChartTypes::PIE); }
};


UCLASS()
class UXYChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	FXYChartData XYChartData;

	virtual void ProcessData() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing XY Chart Data"));
	}

public:
	virtual void Initialize(EChartTypes Type) override { Super::Initialize(EChartTypes::XY); }
	virtual bool SetChartData(const FString& XName, const TArray<float>& XData, const FString& YName, const TArray<float>& YData);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FXYChartData& InputData);
};

UCLASS()
class UXYZChartClass : public UDataClasses
{
	GENERATED_BODY()

private:
	FXYZChartData XYZChartData;

	virtual void ProcessData() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing XYZ Chart Data"));
	}

public:
	virtual void Initialize(EChartTypes Type) override { Super::Initialize(EChartTypes::XYZ); }
	virtual bool SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues);
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual bool SetChartData(const FXYZChartData& InputData);
};
