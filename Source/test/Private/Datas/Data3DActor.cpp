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

//void AData3DActor::OnConstruction(const FTransform& Transform)
//{
//	Super::OnConstruction(Transform);
//	
//}


// Called when the game starts or when spawned
void AData3DActor::BeginPlay()
{
	Super::BeginPlay();

	InitilizeDataManager();
	ClearChildrenActors();

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


// ��� ��Ʈ 
void AData3DActor::GenerateShapeChart(const FShapeChartData& CopiedData)
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Generating ShapeChart"));
	ClearChildrenActors();
	
	TArray<float> ValueArray = CopiedData.Values;
	int32 Numbers = ValueArray.Num();

	// ���ö��� �� ����
	float SplineLength = SplineComponent->GetSplineLength();

	// ��Ʈ �ִ� ����
	float MaxHeight = ArrowComponent->ArrowLength;

	// ���� ���� ����
	float BarSpacing = SplineLength / (Numbers - 1);

	// �� Ÿ�� ��Ʈ ���� ���� �غ�
	if (CopiedData.ChartType == "bar")
	{
		float AverageHeight=0;
		float BarHeightScaler=0;

		bool isPrepareDone = PrepareBarValues(ValueArray, AverageHeight, BarHeightScaler, SplineLength, MaxHeight);

		if (!isPrepareDone)
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor: Preparing Bar Values Failed"));
			return;
		}

		// �� ��Ʈ ����
		for (int32 i = 0; i < Numbers; i++)
		{
			float CurrentValue = CopiedData.Values[i];
			float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
			float ScaledHeight = (CurrentValue+ ScaledDeviation) * BarHeightScaler;

			float Distance = i * BarSpacing;
			
			FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);	

			// �ڼ� ����(��Ʈ ����) ���� UChildActorComponent* �ݺ� ����
			UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);
			NewChildActorComponent->SetupAttachment(RootComponent);

			if (NewChildActorComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("Data3DActor : Creating Bar Child Object : %s"), *NewChildActorComponent->GetName());

				// �ڼ� ������Ʈ ����
				NewChildActorComponent->SetupAttachment(RootComponent);

				//�ڼ� ���� Ŭ���� ����
				NewChildActorComponent->SetChildActorClass(BarBase);

				// �ڼ� ���� ����
				NewChildActorComponent->CreateChildActor();
				
				// �迭�� �߰�
				ChildActorComponents.Add(NewChildActorComponent);

				// �� �޽� ����
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());

				if (ChildBar)
				{
					//UE_LOG(LogTemp, Log, TEXT("Data3DActor : ChildBP : %s"), *ChildBar->AnimationCurve->GetName());

					ChildBar->CreateBarMesh(ScaledHeight);

					//ChildBar->PlayBarAnimation();

					// �̵�
					ChildBar->SetActorRelativeLocation(BarLocation);
					
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Data3DActor : Failed Casting ChildBarBaseActor"));
				}
			}

			FString Label = CopiedData.Labels.IsValidIndex(i) ? CopiedData.Labels[i] : TEXT("No Label");
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Created bar for Label : %s with Height: %f"), *Label, ScaledHeight);
		}
	}
}
// PreperateBar(ValueArray, &AverageHeight, &BarHeightScaler, SplineLength, MaxHeight);
bool AData3DActor::PrepareBarValues(const TArray<float>& ValueArray, float& AverageHeightResult, float&BarHeightScalerResult , int SplineLength, int MaxHeight)
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Preperating Bar Chart"));

	int32 Numbers = ValueArray.Num();

	// ��Ʈ ���
	float sum = 0;
	for (float value : ValueArray)
	{
		sum += value;
	}
	AverageHeightResult = sum / Numbers;

	// ������ ���� �ڵ� �����Ǿ��
	BarHeightScalerResult = MaxHeight / (2 * AverageHeightResult);

	// �α� �����ϸ�, ����ȭ ���� �Ķ���� ���� �� �� �ְ� �� ��

	if (ChildActorComponents.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Child Actors Already Exist"));
		return false;
	}
	else
	{
		return true;
	}
}

void AData3DActor::PlayChildrenAnimation()
{
	if (DataManagerPtr)
	{
		EChartTypes LastType = DataManagerPtr->LastChartType;
		switch (LastType)
		{
		case None:
			break;
		case BAR:
			for (const UChildActorComponent* ChildActorComponent : ChildActorComponents)
			{
				if (ChildActorComponent && ChildActorComponent->GetChildActor())
				{
					ABarBaseActor* BarChildActor = Cast<ABarBaseActor>(ChildActorComponent->GetChildActor());
					BarChildActor->PlayBarAnimation();
				}
			}
			break;
		case LINE:
			break;
		case PIE:
			break;
		case XY:
			break;
		case XYZ:
			break;
		case FREE:
			break;
		default:
			break;
		}
			
	}
	
}

// Base�� ���� ���� ����
void AData3DActor::ClearChildrenActors()
{
	int32 ExistActors = ChildActorComponents.Num();


	for (UChildActorComponent* ChildComponent : ChildActorComponents)
	{
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Children Actor %s cleard"), *ChildComponent->GetChildActor()->GetName());
			ChildComponent->GetChildActor()->Destroy();
			ChildComponent->DestroyComponent();
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
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : LastChartType is %d"), LastType)
		switch (LastType)
		{
		case None:
			break;
		case BAR:
			GenerateShapeChart(DataManagerPtr->ShapeChartData);
			break;
		case LINE:
			break;
		case PIE:
			break;
		case XY:
			break;
		case XYZ:
			break;
		case FREE:
			break;
		default:
			break;
		}

	}
}


