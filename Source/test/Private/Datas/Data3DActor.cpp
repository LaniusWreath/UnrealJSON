	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Data3DActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/BarBaseActor.h"
#include "Components/SplineComponent.h"

AData3DActor::AData3DActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);

	// �����Ϳ��� �ٽ� ������ ��
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	//if (MeshAsset.Succeeded())
	//{
	//	BaseMesh->SetStaticMesh(MeshAsset.Object);
	//	// �޽� �����Ƽ
	//	BaseMesh->SetMobility(EComponentMobility::Movable);
	//}

}

void AData3DActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GenerateShapeChart(TestData);
	
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


// ���� ��Ʈ 
void AData3DActor::GenerateShapeChart(const FShapeChartData& CopiedData)
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Generating ShapeChart"));
		ClearChildrenActors();

	// �� Ÿ�� ��Ʈ ����
	if (CopiedData.ChartType == "bar")
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Generating Bar Chart"));
			
		const float BarWidth = Width_bar;		// ���� ���� �ʺ�
		//const float BarSpacing = Height_bar;	// ���� ���� ����

		int32 Numbers = CopiedData.Values.Num();

		// ���ö��� �� ����
		float SplineLength = SplineComponent->GetSplineLength();
			 
		// ���� ���� ����
		float BarSpacing = SplineLength / (Numbers - 1);

		for (int32 i = 0; i < Numbers; i++)
		{
			float BarHeight = CopiedData.Values[i];
			float Distance = i * BarSpacing;
			
			FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);	

			ABarBaseActor* NewBar = GetWorld()->SpawnActor<ABarBaseActor>(BarBase);
			NewBar->CreateBarMesh(BarHeight);
			NewBar->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			NewBar->SetActorRelativeLocation(BarLocation);

			FString Label = CopiedData.Labels.IsValidIndex(i) ? CopiedData.Labels[i] : TEXT("No Label");
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Created bar for Label : %s with Height: %f"), *Label, BarHeight);
		}
	}
}

// Base�� ���� ���� ����
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


