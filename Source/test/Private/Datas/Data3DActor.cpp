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
		DataManagerPtr = GameInstance->GetDataManager();
		if (DataManagerPtr)
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


// Called every frame
void AData3DActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 막대 차트 데이터 세팅
void AData3DActor::GenerateBarChart()
{
	if (DataManagerPtr)
	{
		const FShapeChartData& CopiedData = DataManagerPtr->ShapeChartData;

		FVector StartPosition(0.f, 0.f, 0.f);
		float BarWidth = 100.f;		// 막대 사이 너비
		float BarSpacing = 120.f;	// 막대 사이 간격

		for (int32 i = 0; i < CopiedData.Labels.Num(); i++)
		{
			float BarHeight = CopiedData.Values[i];

			FVector BarLocation = StartPosition + FVector(BarSpacing * i, 0.f, BarHeight / 2);
			FRotator BarRotation = FRotator::ZeroRotator;

			FActorSpawnParameters SpawnParams;
			AActor* BarActor = GetWorld() -> SpawnActor<AActor>(AActor::StaticClass(), BarLocation, BarRotation, SpawnParams);
			
			if (BarActor)
			{
				BarActor->SetActorScale3D(FVector(BarWidth, BarWidth, BarHeight));

				FString Label = CopiedData.Labels.IsValidIndex(i) ? CopiedData.Labels[i] : TEXT("No Label");
				UE_LOG(LogTemp, Log, TEXT("Data3DActor : Created bar for Label : %s with Height: %f"), *Label, BarHeight);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChartType is not bar or check DataManagerPtr"));
	}
}

void AData3DActor::GetDataFromDataManager()
{
	if (DataManagerPtr)
	{
		EChartTypes LastType = DataManagerPtr->LastChartType;
		switch (LastType)
		{
		case None:
			break;
		case E_SHAPE:
			break;
		case E_XY:
			break;
		case E_XYZ:
			break;
		case E_FREE:
			break;
		default:
			break;
		}
	}

}


