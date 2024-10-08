// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/ChartGenerator.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Datas/BarBaseActor.h"


// �⺻ ��Ʈ ���̽� �ʱ�ȭ
UChartGenerator::UChartGenerator()
{
	//RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	//RootMeshComponent->SetupAttachment(this);
}

void UChartGenerator::BeginPlay()
{

}

// Base�� ���� ���� ����
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
	// ���ö��� �� ���� 
	float SplineLength = SplineComponent->GetSplineLength();
	// ��Ʈ �ִ� ����
	float MaxHeight = ArrowComponent->ArrowLength;

	TArray<float> ValueArray = CopiedData.Values;
	TArray<FString> LabelAarray = CopiedData.Labels;

	// ���ö��� ���� ����
	float SplineSpacing = SplineLength / (ValueArray.Num() - 1);

	// �� Ÿ�� ��Ʈ ���� ���� �غ�
	float AverageHeight = 0;
	float BarHeightScaler = 0;
	PrepareBarValues(ValueArray, AverageHeight, BarHeightScaler, SplineLength, MaxHeight);

	// �� �޽� ����
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
}

bool UBarGenerator::CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing, const float AverageHeight, const float BarHeightScaler)
{
	ClearChildrenActors();

	int32 Numbers = ValueArray.Num();

	// �� ��Ʈ ���� : ValueArray, AverageHeight, BarHeightScaler
	for (int32 i = 0; i < Numbers; i++)
	{
		float CurrentValue = ValueArray[i];
		float ScaledDeviation = (CurrentValue - AverageHeight) * DeviationScaler;
		float ScaledHeight = (CurrentValue + ScaledDeviation) * BarHeightScaler;
		float Distance = i * BarSpacing;

		FVector BarLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		FString LabelName = LabelArray[i];

		// �ڼ� ����(��Ʈ ����) ���� UChildActorComponent* �ݺ� ����
		UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

		if (NewChildActorComponent)
		{
			// �ڼ� ������Ʈ ����
			NewChildActorComponent->SetupAttachment(RootMeshComponent);

			//�ڼ� ���� Ŭ���� ����
			NewChildActorComponent->SetChildActorClass(BarBaseActorBPClass);

			// �ڼ� ���� ����
			NewChildActorComponent->CreateChildActor();

			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Creating Bar Child Object : %s"), *NewChildActorComponent->GetChildActorClass()
			->GetName());

			// �迭�� �߰�
			ChildActorComponents.Add(NewChildActorComponent);

			if (NewChildActorComponent->GetChildActor())
			{
				// ABarBaseAcotr�� UChildActorComponent ĳ����
				ABarBaseActor* ChildBar = Cast<ABarBaseActor>(NewChildActorComponent->GetChildActor());
				if (ChildBar)
				{
					// �� ���ν����� �޽� ����
					ChildBar->CreateBarMesh(ScaledHeight);
					// �� �� �ؽ�Ʈ ������ ����
					ChildBar->CreateTextMeshLabel(LabelName); //������� �ϸ� ��
					// �� �� �ؽ�Ʈ ������ ����
					ChildBar->CreateTextMeshValue(CurrentValue, ScaledHeight);
					// �̵�
					ChildBar->SetActorRelativeLocation(BarLocation);
					// �ִϸ��̼�
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


