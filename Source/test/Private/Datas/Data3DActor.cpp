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
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Instancing %s"), *GetName());

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	
	// �����Ϳ��� �ٽ� ������ ��
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	//if (MeshAsset.Succeeded())
	//{
	//	BaseMesh->SetStaticMesh(MeshAsset.Object);
	//	// �޽� �����Ƽ
	//	BaseMesh->SetMobility(EComponentMobility::Movable);
	//}
}


void AData3DActor::UpdateInEditor()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Debuging Chart Instance : %s"), *GetName());
	GenerateShapeChart(TestShapeData);
}

void AData3DActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
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

		// Array ������
		int32 Numbers = CopiedData.Values.Num();

		// ���ö��� �� ����
		float SplineLength = SplineComponent->GetSplineLength();

		// ���� ���� ����
		float BarSpacing = SplineLength / (Numbers - 1);

		// ��Ʈ �ִ� ����
		float MaxHeight = ArrowComponent->ArrowLength;

		// ��Ʈ ���
		float sum = 0;
		for (float value : CopiedData.Values)
		{
			sum += value;
		}
		float AverageHeight = sum/Numbers;
		float BarHeightScaler = MaxHeight / (2 * AverageHeight);

		// �α� �����ϸ�, ����ȭ ���� �Ķ���� ���� �� �� �ְ� �� ��
		// 

		for (int32 i = 0; i < Numbers; i++)
		{
			float CurrentValue = CopiedData.Values[i];
			float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
			float ScaledHeight = (CurrentValue+ ScaledDeviation) * BarHeightScaler;

			float Distance = i * BarSpacing;
			
			FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);	

			// �ڼ� ����(��Ʈ ����) ���� ���� ����
			UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

			if (NewChildActorComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("Data3DActor : Creating Bar Child Object : %s"), *NewChildActorComponent->GetName());

				// �θ� attach
				NewChildActorComponent->SetupAttachment(RootComponent);

				//�ڼ� ���� Ŭ���� ����
				NewChildActorComponent->SetChildActorClass(BarBase);

				// �ڼ� ���� ����
				NewChildActorComponent->RegisterComponent();;
				NewChildActorComponent->CreateChildActor();
				
				// �迭�� �߰�
				ChildActorComponents.Add(NewChildActorComponent);

				// �����Ϳ� �޾� ��Ʈ ����
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());
				ChildBar->CreateBarMesh(ScaledHeight);
				ChildBar->SetActorRelativeLocation(BarLocation);
			}

			FString Label = CopiedData.Labels.IsValidIndex(i) ? CopiedData.Labels[i] : TEXT("No Label");
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Created bar for Label : %s with Height: %f"), *Label, ScaledHeight);
		}
	}
}


// Base�� ���� ���� ����
void AData3DActor::ClearChildrenActors()
{
	//TArray<AActor*> AttachedActors;
	//GetAttachedActors(AttachedActors);

	//for (AActor* Actor : AttachedActors)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Children Actor %s cleard"), *Actor->GetName());
	//	Actor->Destroy();
	//}
	//UE_LOG(LogTemp, Log, TEXT("All Children Actors cleard"));

	for (UChildActorComponent* ChildComponent : ChildActorComponents)
	{
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Children Actor %s cleard"), *ChildComponent->GetChildActor()->GetName());
			ChildComponent->GetChildActor()->Destroy();
		}
	}
	UE_LOG(LogTemp, Log, TEXT("All Children Actors cleard"));

	ChildActorComponents.Empty();
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


