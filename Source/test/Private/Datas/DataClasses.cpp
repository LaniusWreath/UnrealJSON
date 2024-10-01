// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/DataClasses.h"

const EChartTypes UDataClasses::GetChartType() const
{
    return ChartType;
}

bool UShapeChartClass::SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values)
{
    bool result = true;

    ShapeChartData.ChartType = ChartTypeName;
    ShapeChartData.XName = XName;
    ShapeChartData.Labels = Labels;
    ShapeChartData.YName = YName;
    ShapeChartData.Values = Values;
    
    if (ShapeChartData.ChartType == "")
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.ChartType is Empty"), *this->GetName());
        result = false;
    }
    if (ShapeChartData.XName == "" || ShapeChartData.YName == "")
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.Name is Empty"), *this->GetName());
        result = false;
    }
    if (ShapeChartData.Labels.Num() == 0 || ShapeChartData.Values.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.Data is Empty"), *this->GetName());
        result = false;
    }

    return result;
}

bool UShapeChartClass::SetChartData(const FShapeChartData& InputData)
{
    bool result = true;
    ShapeChartData = InputData;
    if (ShapeChartData.ChartType == "")
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.ChartType is Empty"), *this->GetName());
        result = false;
    }
    if (ShapeChartData.XName == "" || ShapeChartData.YName == "")
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.Name is Empty"), *this->GetName());
        result = false;
    }
    if (ShapeChartData.Labels.Num() == 0 || ShapeChartData.Values.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("UBarShapeComponent.cpp : %s.Data is Empty"), *this->GetName());
        result = false;
    }

    return false;
}

const TArray<float>& UShapeChartClass::GetBarChartDataValues() const
{
    if (ShapeChartData.Values.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("DataClass.cpp : %s Values is empty"), *this->GetName());
    }
    return ShapeChartData.Values;

}

bool UXYChartClass::SetChartData(const FXYChartData& InputData)
{
    return false;
}

bool UXYChartClass::SetChartData(const FString& XName, const TArray<float>& XData, const FString& YName, const TArray<float>& YData)
{
    return false;
}

bool UXYZChartClass::SetChartData(const FXYZChartData& InputData)
{
    return false;
}

bool UXYZChartClass::SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues)
{
    return false;
}

bool UShapeChartBarClass::SetChartData(const FShapeChartData& InputData)
{
    bool result = false;
    if (Super::SetChartData(InputData))
    {
        result = true;
    }
    
    return result;
}

bool UShapeChartBarClass::SetChartData(const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values)
{
    bool result = false;
    if (Super::SetChartData(ChartTypeName, XName, Labels, YName, Values))
    {
        result = true;
    }

    return result;
}
