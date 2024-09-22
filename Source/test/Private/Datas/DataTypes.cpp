// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/DataTypes.h"

TMap<FString, int32> DataTypes::MapChartTypes = 
{
	{TEXT("BAR"), 1},
	{TEXT("LINE"), 2},
	{TEXT("PIE"), 3},
	{TEXT("XY"), 4},
	{TEXT("XYZ"), 5},
	{TEXT("FREE"), 6}
};

DataTypes::DataTypes()
{
	
}

DataTypes::~DataTypes()
{
}