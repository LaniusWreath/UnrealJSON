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

	
	// 에디터에서 다시 설정할 것
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	//if (MeshAsset.Succeeded())
	//{
	//	BaseMesh->SetStaticMesh(MeshAsset.Object);
	//	// 메시 모빌리티
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
		float BarHeightScaler = MaxHeight / (2 * AverageHeight);

		// 로그 스케일링, 정규화 따로 파라미터 빼서 고를 수 있게 할 것
		
		if (ChildActorComponents.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Child Actors Already Exist"));
			return;
		}


		for (int32 i = 0; i < Numbers; i++)
		{
			float CurrentValue = CopiedData.Values[i];
			float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
			float ScaledHeight = (CurrentValue+ ScaledDeviation) * BarHeightScaler;

			float Distance = i * BarSpacing;
			
			FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);	

			// 자손 액터(차트 액터) 넣을 변수 생성
			UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);
			NewChildActorComponent->SetupAttachment(RootComponent);

			if (NewChildActorComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("Data3DActor : Creating Bar Child Object : %s"), *NewChildActorComponent->GetName());

				// 부모에 attach
				NewChildActorComponent->SetupAttachment(RootComponent);

				//자손 액터 클래스 설정
				NewChildActorComponent->SetChildActorClass(BarBase);

				// 자손 액터 생성
				NewChildActorComponent->CreateChildActor();
				
				// 배열에 추가
				ChildActorComponents.Add(NewChildActorComponent);

				// 바 메쉬 생성
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());

				if (ChildBar)
				{
					//UE_LOG(LogTemp, Log, TEXT("Data3DActor : ChildBP : %s"), *ChildBar->AnimationCurve->GetName());

					ChildBar->CreateBarMesh(ScaledHeight);

					//ChildBar->PlayBarAnimation();

					// 이동
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
			for (UChildActorComponent* ChildActorComponent : ChildActorComponents)
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

// Base에 붙은 액터 삭제
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


