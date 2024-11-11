// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMDataTypes.h"

// TEXT �Ǵ� String�� Enum�� �����ϱ� ���� TMap
TMap<FString, EJCMChartTypes> JCMDataTypes::JCMMapChartTypes = 
{
	{TEXT("BAR"), EJCMChartTypes::BAR},
	{TEXT("LINE"), EJCMChartTypes::LINE},
	{TEXT("PIE"), EJCMChartTypes::PIE},
	{TEXT("XY"), EJCMChartTypes::XY},
	{TEXT("XYZ"), EJCMChartTypes::XYZ},
	{TEXT("FREE"), EJCMChartTypes::FREE}
};

JCMDataTypes::JCMDataTypes()
{
	
}

JCMDataTypes::~JCMDataTypes()
{
}