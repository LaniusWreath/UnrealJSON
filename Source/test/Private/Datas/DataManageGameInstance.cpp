// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/DataManageGameInstance.h"
#include "Datas/DataManager.h"

void UDataManageGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("Initialize DataManageGameInstance"));
	DataManagerInstance = NewObject<UDataManager>(this);
	if (DataManagerInstance)
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
	if (DataManagerInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("DataManageGameInstace : DataManager Getter Excuted"));
		return DataManagerInstance;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DataManageGameInstance : DataManager instance is invalid"));
		return nullptr;
	}
}
