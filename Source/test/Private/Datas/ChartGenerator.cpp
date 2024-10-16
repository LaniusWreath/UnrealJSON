// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/ChartGenerator.h"
#include "Components/SplineComponent.h"
#include "Datas/BarBaseActor.h"
#include "Algo/MaxElement.h"
#include "Components/TextRenderComponent.h"


// 기본 차트 베이스 초기화
UChartGenerator::UChartGenerator()
{
	ChildActorContainComponent= CreateDefaultSubobject<USceneComponent>(TEXT("childActorContainComponent"));
	ChildActorContainComponent->SetupAttachment(this);
	UE_LOG(LogTemp, Log, TEXT("%s"), *(GetAttachmentRoot()->GetName()));
	ChildActorContainComponent->SetMobility(EComponentMobility::Movable);
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
	// 차트 들어가는 각 컴포넌트 인스턴스 생성만, Attach는 Data3DActor에서 할 것
	SplineComponent_length = CreateDefaultSubobject<USplineComponent>(TEXT("LengthSplineComponent"));
	SplineComponent_length->SetMobility(EComponentMobility::Movable);

	SplineComponent_height = CreateDefaultSubobject<USplineComponent>(TEXT("HeightSplineComponent"));
	SplineComponent_height->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SplineComponent_height->SetMobility(EComponentMobility::Movable);
}

void UBarGenerator::SetBarSourceActor(const TSubclassOf<ABarBaseActor>& SourceActor)
{
	BarBaseActorBPClass = SourceActor;
}

// 바 차트 생성 함수
void UBarGenerator::GenerateBarChart(const FShapeChartData& CopiedData)
{
	// 스플라인 총 길이 
	float SplineLength = SplineComponent_length->GetSplineLength();
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : SplineComponent is %f"), SplineLength);

	// 차트 최대 높이
	float MaxHeight = SplineComponent_height->GetSplineLength();
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : SplineHeight is %f"), MaxHeight);

	TArray<float> ValueArray = CopiedData.Values;
	TArray<FString> LabelAarray = CopiedData.Labels;

	// 스플라인 사이 간격
	float SplineSpacing = SplineLength / (ValueArray.Num() - 1);

	// 바 타입 차트 생성 사전 준비
	float BarHeightScaler = 0;
	float BarPadding = 0;
	PrepareBarValues(ValueArray, BarHeightScaler, BarPadding, MaxHeight);
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator: PrepareBarValues() result, BarHeightScaler : %f"), BarHeightScaler);

	// 바 메시 생성
	bool isGenerateDone = CreateBar(ValueArray, LabelAarray, SplineSpacing, BarPadding, BarHeightScaler);
	if (!isGenerateDone)
	{
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Generating Bar Failed"));
		return;
	}
}

// 바 차트 생성 전 전처리 함수
void UBarGenerator::PrepareBarValues(const TArray<float>& ValueArray, float& BarHeightScalerResult, float& BarPaddingResult,
	const float MaxHeight)
{
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Preperating Bar Chart"));

	int32 Numbers = ValueArray.Num();

	float Range = *Algo::MaxElement(ValueArray) - *Algo::MinElement(ValueArray);
	//UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Range is : %f - %f = %f"), , *Algo::MinElement(ValueArray), Range);
	// 100 * 0.9 / 500
	BarHeightScalerResult = MaxHeight * CustomScaleValue / Range;
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Preparing Bar Height Scaler : %f * %f / %f"), MaxHeight, CustomScaleValue, Range);

	BarPaddingResult = MaxHeight * CustomPaddingScaleValue;
	// 로그 스케일링, 정규화 따로 파라미터 빼서 고를 수 있게 할 것
}

bool UBarGenerator::CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing, 
	const float BarPaddingScaler, const float BarHeightScaler)
{
	ClearChildrenActors();

	int32 Numbers = ValueArray.Num();

	// 바 차트 생성 : ValueArray, AverageHeight, BarHeightScaler
	for (int32 i = 0; i < Numbers; i++)
	{
		float CurrentValue = ValueArray[i];
		float ScaledHeight = CurrentValue * BarHeightScaler;
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : CurrentValue : %f, BarHeightScaler : %f, ScaledHeight : %f"), 
			CurrentValue, BarHeightScaler ,ScaledHeight);
		float Distance = i * BarSpacing;

		FVector BarLocation = SplineComponent_length->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		FString LabelName = LabelArray[i];

		// 자손 액터(차트 액터) 넣을 UChildActorComponent* 반복 생성
		UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

		if (NewChildActorComponent)
		{
			// 자손 컴포넌트 부착
			NewChildActorComponent->SetupAttachment(ChildActorContainComponent);

			//자손 액터 클래스 설정
			NewChildActorComponent->SetChildActorClass(BarBaseActorBPClass);

			// 자손 액터 생성
			NewChildActorComponent->CreateChildActor();

			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Creating Bar Child Object : %s"), 
				*NewChildActorComponent->GetChildActorClass()->GetName());

			// 배열에 추가
			ChildActorComponents.Add(NewChildActorComponent);

			if (NewChildActorComponent->GetChildActor())
			{
				// ABarBaseAcotr로 UChildActorComponent 캐스팅
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());
				if (ChildBar)
				{
					// 바 프로시저럴 메쉬 생성
					ChildBar->CreateMesh(ScaledHeight);
					// 바 라벨 텍스트 렌더러 생성
					ChildBar->InitializeTextMeshLabel(LabelName); 
					// 바 값 텍스트 렌더러 생성
					ChildBar->InitializeTextMeshValue(CurrentValue, ScaledHeight);
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


