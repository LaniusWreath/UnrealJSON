// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/DataTypes.h"

// TEXT 또는 String을 Enum에 매핑하기 위한 TMap
TMap<FString, EChartTypes> DataTypes::MapChartTypes = 
{
	{TEXT("BAR"), EChartTypes::BAR},
	{TEXT("LINE"), EChartTypes::LINE},
	{TEXT("PIE"), EChartTypes::PIE},
	{TEXT("XY"), EChartTypes::XY},
	{TEXT("XYZ"), EChartTypes::XYZ},
	{TEXT("FREE"), EChartTypes::FREE}
};

DataTypes::DataTypes()
{
	
}

DataTypes::~DataTypes()
{
}