// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManageGameInstance.h"
#include "Datas/DataManager.h"

void UDataManageGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("Initialize DataManageGameInstance"));
	DataManager = NewObject<UDataManager>(this);
	if (DataManager)
	{
		UE_LOG(LogTemp, Log, TEXT("DataManageGameInstance Initialized"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("DataManageGameInstance Initialize Failed"));
	}
}

UDataManager* UDataManageGameInstance::GetDataManager() const
{
	if (DataManager)
	{
		UE_LOG(LogTemp, Log, TEXT("DataManageGameInstace : DataManager Getter Excuted"));
		return DataManager;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DataManageGameInstance : DataManager instance is invalid"));
		return nullptr;
	}
}
