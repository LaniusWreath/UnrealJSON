// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Data3DActor.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts or when spawned
void AData3DActor::BeginPlay()
{

	Super::BeginPlay();
	
	InitilizeDataManager();

}


void AData3DActor::InitilizeDataManager()
{
	// Connect to GameInstance and Get DataManager Reference
	UDataManageGameInstance* GameInstance = Cast<UDataManageGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance && GameInstance->DataManager)
	{
		DataManager = GameInstance->GetDataManager();
		if (DataManager)
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : DataManager has referenced well"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't get DataManager Reference"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find GameInstance or DataManager"));
	}
}

// Cache Data From DataManager
void AData3DActor::CacheJSONDatas()
{
	if (DataManager)
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Get JSON Data Reference from DataManager"));
		ParsedJSONData = DataManager->GetJSONParsedData();
	}
}

// Called every frame
void AData3DActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AData3DActor::ConstructGraph()
{
	// 기본 그래프 구성
}


