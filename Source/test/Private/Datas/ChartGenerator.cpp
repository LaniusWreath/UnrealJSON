// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/ChartGenerator.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Datas/BarBaseActor.h"


// 기본 차트 베이스 초기화
UChartGenerator::UChartGenerator()
{
	//RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	//RootMeshComponent->SetupAttachment(this);
}

void UChartGenerator::BeginPlay()
{

}

// Base에 붙은 액터 삭제
void UChartGenerator::ClearChildrenActors()
{
	int32 ExistActors = ChildActorComponents.Num();

	for (UChildActorComponent* ChildComponent : ChildActorComponents)
	{
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Children Actor %s cleard"), *ChildComponent->GetChildActor()->GetName());
			ChildComponent->GetChildActor()->Destroy();
			ChildComponent->DestroyComponent();
		}
	}
	UE_LOG(LogTemp, Log, TEXT("All Children Actors cleard"));

	ChildActorComponents.Empty();
}

////////////////////////////////////////////////////////////////////////////////////////

UBarGenerator::UBarGenerator()
{

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootMeshComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootMeshComponent);
	ArrowComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	
}

void UBarGenerator::SetBarSourceActor(const TSubclassOf<ABarBaseActor>& SourceActor)
{
	BarBaseActorBPClass = SourceActor;
}

void UBarGenerator::GenerateBarChart(const FShapeChartData& CopiedData)
{
	// 스플라인 총 길이 
	float SplineLength = SplineComponent->GetSplineLength();
	// 차트 최대 높이
	float MaxHeight = ArrowComponent->ArrowLength;

	TArray<float> ValueArray = CopiedData.Values;
	TArray<FString> LabelAarray = CopiedData.Labels;

	// 스플라인 사이 간격
	float SplineSpacing = SplineLength / (ValueArray.Num() - 1);

	// 바 타입 차트 생성 사전 준비
	float AverageHeight = 0;
	float BarHeightScaler = 0;
	PrepareBarValues(ValueArray, AverageHeight, BarHeightScaler, SplineLength, MaxHeight);

	// 바 메시 생성
	bool isGenerateDone = CreateBar(ValueArray, LabelAarray, SplineSpacing, AverageHeight, BarHeightScaler);
	if (!isGenerateDone)
	{
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Generating Bar Failed"));
		return;
	}
}



void UBarGenerator::PrepareBarValues(const TArray<float>& ValueArray, float& AverageHeightResult, float& BarHeightScalerResult, const int SplineLength, const int MaxHeight)
{
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Preperating Bar Chart"));

	int32 Numbers = ValueArray.Num();

	// 차트 평균
	float sum = 0;
	for (float value : ValueArray)
	{
		sum += value;
	}
	AverageHeightResult = sum / Numbers;

	// 스케일 결정 코드 수정되어야
	BarHeightScalerResult = MaxHeight / (2 * AverageHeightResult);

	// 로그 스케일링, 정규화 따로 파라미터 빼서 고를 수 있게 할 것
}

bool UBarGenerator::CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing, const float AverageHeight, const float BarHeightScaler)
{
	ClearChildrenActors();

	int32 Numbers = ValueArray.Num();

	// 바 차트 생성 : ValueArray, AverageHeight, BarHeightScaler
	for (int32 i = 0; i < Numbers; i++)
	{
		float CurrentValue = ValueArray[i];
		float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
		float ScaledHeight = (CurrentValue + ScaledDeviation) * BarHeightScaler;
		float Distance = i * BarSpacing;

		FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		FString LabelName = LabelArray[i];

		// 자손 액터(차트 액터) 넣을 UChildActorComponent* 반복 생성
		UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

		if (NewChildActorComponent)
		{
			// 자손 컴포넌트 부착
			NewChildActorComponent->SetupAttachment(RootMeshComponent);

			//자손 액터 클래스 설정
			NewChildActorComponent->SetChildActorClass(BarBaseActorBPClass);

			// 자손 액터 생성
			NewChildActorComponent->CreateChildActor();

			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Creating Bar Child Object : %s"), *NewChildActorComponent->GetChildActorClass()
			->GetName());

			// 배열에 추가
			ChildActorComponents.Add(NewChildActorComponent);

			if (NewChildActorComponent->GetChildActor())
			{
				// ABarBaseAcotr로 UChildActorComponent 캐스팅
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());
				if (ChildBar)
				{
					// 바 프로시저럴 메쉬 생성
					ChildBar->CreateBarMesh(ScaledHeight);
					// 바 라벨 텍스트 렌더러 생성
					ChildBar->CreateTextMeshLabel(LabelName); //여기부터 하면 됨
					// 바 값 텍스트 렌더러 생성
					ChildBar->CreateTextMeshValue(CurrentValue, ScaledHeight);
					// 이동
					ChildBar->SetActorRelativeLocation(BarLocation);
					// 애니메이션
					ChildBar->PlayBarAnimation();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ChartGenerator: Failed Casting ChildBarBaseActor"));
					return false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ChartGenerator : Failed NewChildActorComponent->GetChildActor()"));
			}

		}

		UE_LOG(LogTemp, Log, TEXT("ChartGenerator: Created bar Number with Height: %f"), ScaledHeight);
	}
	return true;
}


