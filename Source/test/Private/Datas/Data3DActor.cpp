	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Data3DActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/DataManageGameInstance.h"
#include "Datas/BarBaseActor.h"
#include "Datas/DataManager.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"

AData3DActor::AData3DActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));


	// 에디터에서 다시 설정할 것
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	//if (MeshAsset.Succeeded())
	//{
	//	BaseMesh->SetStaticMesh(MeshAsset.Object);
	//	// 메시 모빌리티
	//	BaseMesh->SetMobility(EComponentMobility::Movable);
	//}

}


void AData3DActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GenerateShapeChart(TestShapeData);
	
}


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


// 막대 차트 
void AData3DActor::GenerateShapeChart(const FShapeChartData& CopiedData)
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Generating ShapeChart"));
		ClearChildrenActors();

	// 바 타입 차트 생성
	if (CopiedData.ChartType == "bar")
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Generating Bar Chart"));

		// Array 사이즈
		int32 Numbers = CopiedData.Values.Num();

		// 스플라인 총 길이
		float SplineLength = SplineComponent->GetSplineLength();
		// 막대 사이 간격
		float BarSpacing = SplineLength / (Numbers - 1);

		// 차트 최대 높이
		float MaxHeight = ArrowComponent->ArrowLength;

		// 차트 평균
		float sum = 0;
		for (float value : CopiedData.Values)
		{
			sum += value;
		}
		float AverageHeight = sum/Numbers;
		float Scaler = MaxHeight / (2 * AverageHeight);

		for (int32 i = 0; i < Numbers; i++)
		{
			float CurrentValue = CopiedData.Values[i];
			float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
			float ScaledHeight = (CurrentValue+ ScaledDeviation) * Scaler;

			float Distance = i * BarSpacing;
			
			FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);	

			ABarBaseActor* NewBar = GetWorld()->SpawnActor<ABarBaseActor>(BarBase);
			NewBar->CreateBarMesh(ScaledHeight);
			NewBar->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			NewBar->SetActorRelativeLocation(BarLocation);

			FString Label = CopiedData.Labels.IsValidIndex(i) ? CopiedData.Labels[i] : TEXT("No Label");
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Created bar for Label : %s with Height: %f"), *Label, ScaledHeight);
		}
	}
}

// Base에 붙은 액터 삭제
void AData3DActor::ClearChildrenActors()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Children Actor %s cleard"), *Actor->GetName());
		Actor->Destroy();
	}
	UE_LOG(LogTemp, Log, TEXT("All Children Actors cleard"));
}


void AData3DActor::GetDataAndCreateChart()
{
	if (DataManagerPtr)
	{
		EChartTypes LastType = DataManagerPtr->LastChartType;
		switch (LastType)
		{
		case None:
			break;
		case E_SHAPE:
			GenerateShapeChart(DataManagerPtr->ShapeChartData);

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


