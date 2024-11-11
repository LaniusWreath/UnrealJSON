// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMChartGenerator.h"
#include "Components/SplineComponent.h"
#include "Datas/JCMBarBaseActor.h"
#include "Algo/MaxElement.h"
#include "Components/TextRenderComponent.h"


// �⺻ ��Ʈ ���̽� �ʱ�ȭ
UJCMChartGenerator::UJCMChartGenerator()
{
	ChildActorContainComponent= CreateDefaultSubobject<USceneComponent>(TEXT("childActorContainComponent"));
	ChildActorContainComponent->SetupAttachment(this);
	ChildActorContainComponent->SetMobility(EComponentMobility::Movable);
}

void UJCMChartGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Base�� ���� ���� ����
void UJCMChartGenerator::ClearChildrenActors()
{
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

UJCMChartGeneratorBar::UJCMChartGeneratorBar()
{
	// ��Ʈ ���� �� ������Ʈ �ν��Ͻ� ������, Attach�� Data3DActor���� �� ��
	SplineComponent_length = CreateDefaultSubobject<USplineComponent>(TEXT("LengthSplineComponent"));
	SplineComponent_length->SetMobility(EComponentMobility::Movable);
	SplineComponent_length->SetupAttachment(this);

	SplineComponent_height = CreateDefaultSubobject<USplineComponent>(TEXT("HeightSplineComponent"));
	SplineComponent_height->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SplineComponent_height->SetMobility(EComponentMobility::Movable);
	SplineComponent_height->SetupAttachment(this);
}

void UJCMChartGeneratorBar::SetBarSourceActor(const TSubclassOf<AJCMBarBaseActor>& SourceActor)
{
	if (!SourceActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("JCMChartGenrator : Setting BarSource Actor Failed"));
		return;
	}
	BarBaseActorBPClass = SourceActor;
}

void UJCMChartGeneratorBar::SetAttachComponents(USceneComponent* TargetComponentInstance)
{
	ChildActorContainComponent->SetupAttachment(TargetComponentInstance);
	SplineComponent_height->SetupAttachment(TargetComponentInstance);
	SplineComponent_length->SetupAttachment(TargetComponentInstance);
}

// ChildActorComponent�� ������ BarBaseActor �ν��Ͻ� ����
void UJCMChartGeneratorBar::ClearChildrenActors()
{
	for (UChildActorComponent* ChildComponent : ChildActorComponents)
	{
		// childActor������Ʈ ����
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			// !! ���� : UChildActorComponent�� Ŭ������ �������� ��, �ش� ������Ʈ ��ü�� ������ Ŭ������ ���ϴ� �� �ƴ�.
			// UChildActorComponent�� ������ Ŭ������ ��� �����̳� Ŭ������ �� ��. (���� get�Լ��� �ν��Ͻ� ȣ���ؾ� ��.)
			AJCMBarBaseActor* ChildBarComponent = Cast<AJCMBarBaseActor>(ChildComponent->GetChildActor());
			ChildBarComponent->ClearCustomMeshes();
			ChildBarComponent->ClearSpawnTimerHandle();

			ChildComponent->GetChildActor()->Destroy();
			ChildComponent->DestroyComponent();

		}
	}
	UE_LOG(LogTemp, Log, TEXT("All Children Actors cleard"));

	ChildActorComponents.Empty();
}

// �� ��Ʈ ���� �Լ�
void UJCMChartGeneratorBar::GenerateBarChart(const FJCMChartDataShape& CopiedData, bool bGenerateMeshAtSplinePoint)
{
	// ���ö��� �� ���� 
	float SplineLength = SplineComponent_length->GetSplineLength();
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : SplineComponent is %f"), SplineLength);

	// ��Ʈ �ִ� ����
	float MaxHeight = SplineComponent_height->GetSplineLength();
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator : SplineHeight is %f"), MaxHeight);

	TArray<float> ValueArray = CopiedData.Values;
	TArray<FString> LabelAarray = CopiedData.Labels;

	// ���ö��� ���� ����
	float SplineSpacing = SplineLength / (ValueArray.Num() - 1);

	// �� Ÿ�� ��Ʈ ���� ���� �غ�
	float BarHeightScaler = 0;
	float BarPadding = 0;
	PrepareBarValues(ValueArray, BarHeightScaler, BarPadding, MaxHeight);
	UE_LOG(LogTemp, Log, TEXT("ChartGenerator: PrepareBarValues() result, BarHeightScaler : %f"), BarHeightScaler);

	bool isGenerateDone;
	if (!bGenerateMeshAtSplinePoint)
	{	
		// ���ö��� �յ��ϰ� �����Ͽ� �޽� ����
		isGenerateDone = CreateBar(ValueArray, LabelAarray, SplineSpacing, BarPadding, BarHeightScaler);
	}
	else
	{
		// ���ö��� ����Ʈ�� �޽� ����
		isGenerateDone = CreateBarAlongSplinePoint(ValueArray, LabelAarray, BarPadding, BarHeightScaler);
	}
	// �� �޽� ����
	if (!isGenerateDone)
	{
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Generating Bar Failed"));
		return;
	}
}

// �� ��Ʈ ���� �� ��ó�� �Լ�
void UJCMChartGeneratorBar::PrepareBarValues(const TArray<float>& ValueArray, float& BarHeightScalerResult, float& BarPaddingResult,
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
	// �α� �����ϸ�, ����ȭ ���� �Ķ���� ���� ���� �� �ְ� �� ��
}

bool UJCMChartGeneratorBar::CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing,
	const float BarPaddingScaler, const float BarHeightScaler)
{
	ClearChildrenActors();
	int32 Numbers = ValueArray.Num();

	// �� ��Ʈ ���� : ValueArray, AverageHeight, BarHeightScaler
	for (int32 i = 0; i < Numbers; i++)
	{
		float CurrentValue = ValueArray[i];
		float ScaledHeight = CurrentValue * BarHeightScaler;
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : CurrentValue : %f, BarHeightScaler : %f, ScaledHeight : %f"), 
			CurrentValue, BarHeightScaler ,ScaledHeight);
		float Distance = i * BarSpacing;

		// ���ö��� ���� ��ǥ�� ���� ������ ��ġ���� �� ��Ʈ �޽��� ��ǥ�� ���� �������� �߰� �ȵ�.
		FVector BarLocation = SplineComponent_length->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		FString LabelName = LabelArray[i];

		// �ڼ� ����(��Ʈ ����) ���� UChildActorComponent* �ݺ� ����
		UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

		if (NewChildActorComponent)
		{
			// �ڼ� ������Ʈ ����
			//NewChildActorComponent->SetupAttachment(ChildActorContainComponent);
			NewChildActorComponent->AttachToComponent(ChildActorContainComponent, FAttachmentTransformRules::KeepRelativeTransform);

			//�ڼ� ���� Ŭ���� ����
			NewChildActorComponent->SetChildActorClass(BarBaseActorBPClass);

			// �ڼ� ���� ����
			NewChildActorComponent->CreateChildActor();

			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Creating Bar Child Object : %s"), 
				*NewChildActorComponent->GetChildActorClass()->GetName());

			// �迭�� �߰�
			ChildActorComponents.Add(NewChildActorComponent);

			if (NewChildActorComponent->GetChildActor())
			{
				// ABarBaseAcotr�� UChildActorComponent ĳ����
				AJCMBarBaseActor* ChildBar = Cast<AJCMBarBaseActor>(NewChildActorComponent->GetChildActor());
				if (ChildBar)
				{
					// �̵� : �̵� ���� ������� ���� ��ǥ�� ������
					ChildBar->SetActorRelativeLocation(BarLocation);
					// �� ���ν����� �޽� ����
					ChildBar->CreateMesh(ScaledHeight, CurrentValue);
					// �� �� �ؽ�Ʈ ������ �� �ʱ�ȭ
					ChildBar->InitializeTextMeshLabel(LabelName); 
					// �� �� �ؽ�Ʈ ������ �� �ʱ�ȭ
					ChildBar->InitializeTextMeshValue(CurrentValue);
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

// ���ö��� ������Ʈ�� �� ���� ���ڶ� �� target��ŭ ������
void UJCMChartGeneratorBar::AddSplinePoint(USplineComponent* SplineComponent, int TargetCount)
{
	int32 SplinePointCount = SplineComponent->GetNumberOfSplinePoints();
	if (TargetCount > SplinePointCount)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ChartGenerator : CreateBarAlongSplinePoint Value Count %d dosen't same as SplinePoint Count %d"),
			TargetCount, SplinePointCount);
		UE_LOG(LogTemp, Warning,
			TEXT("Adding Extra Spline Point Automatically"));

		FVector LastSplinePointVector = SplineComponent_length->
			GetLocationAtSplinePoint(SplinePointCount - 1, ESplineCoordinateSpace::Local);
		UE_LOG(LogTemp, Log, TEXT("LastSplinePointVector : %f, %f, %f"),
			LastSplinePointVector.X, LastSplinePointVector.Y, LastSplinePointVector.Z);

		FVector UnitOffsetVector =
			LastSplinePointVector - SplineComponent_length->
			GetLocationAtSplinePoint(SplinePointCount - 2, ESplineCoordinateSpace::Local);
		UE_LOG(LogTemp, Log, TEXT("UnitOffsetVector : %f, %f, %f"), UnitOffsetVector.X, UnitOffsetVector.Y, UnitOffsetVector.Z);

		for (int i = 0; i < TargetCount - SplinePointCount; i++)
		{
			SplineComponent_length->AddSplinePoint(LastSplinePointVector + (UnitOffsetVector * (i + 1)),
				ESplineCoordinateSpace::Local);
		}
	}
}


bool UJCMChartGeneratorBar::CreateBarAlongSplinePoint(const TArray<float>& ValueArray, const TArray<FString>& LabelArray,
	const float BarPaddingScaler, const float BarHeightScaler)
{
	// ���� bar ��ü ����
	ClearChildrenActors();

	int32 Numbers = ValueArray.Num();
	int32 SplinePointCount = SplineComponent_length->GetNumberOfSplinePoints();		// ���ö��� ����Ʈ ����

	// ������ ���� ���ö��� ����Ʈ ���� ����
	AddSplinePoint(SplineComponent_length, Numbers);

	// �� ��Ʈ ���� : ValueArray, AverageHeight, BarHeightScaler
	for (int32 i = 0; i < Numbers; i++)
	{
		float CurrentValue = ValueArray[i];
		float ScaledHeight = CurrentValue * BarHeightScaler;
		UE_LOG(LogTemp, Log, TEXT("ChartGenerator : CurrentValue : %f, BarHeightScaler : %f, ScaledHeight : %f"),
			CurrentValue, BarHeightScaler, ScaledHeight);

		// ���ö��� ���� ��ǥ�� ���� ������ ��ġ���� �� ��Ʈ �޽��� ��ǥ�� ���� �������� �߰� �ȵ�.
		FVector BarLocation = SplineComponent_length->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FString LabelName = LabelArray[i];

		// �ڼ� ����(��Ʈ ����) ���� UChildActorComponent* �ݺ� ����
		UChildActorComponent* NewChildActorComponent = NewObject<UChildActorComponent>(this);

		if (NewChildActorComponent)
		{
			// �ڼ� ������Ʈ ����
			//NewChildActorComponent->SetupAttachment(ChildActorContainComponent);
			NewChildActorComponent->AttachToComponent(ChildActorContainComponent, FAttachmentTransformRules::KeepRelativeTransform);

			//�ڼ� ���� Ŭ���� ����
			NewChildActorComponent->SetChildActorClass(BarBaseActorBPClass);

			// �ڼ� ���� ����
			NewChildActorComponent->CreateChildActor();

			UE_LOG(LogTemp, Log, TEXT("ChartGenerator : Creating Bar Child Object : %s"),
				*NewChildActorComponent->GetChildActorClass()->GetName());

			// �迭�� �߰�
			ChildActorComponents.Add(NewChildActorComponent);

			if (NewChildActorComponent->GetChildActor())
			{
				// ABarBaseAcotr�� UChildActorComponent ĳ����
				AJCMBarBaseActor* ChildBar = Cast<AJCMBarBaseActor>(NewChildActorComponent->GetChildActor());
				if (ChildBar)
				{
					// �̵� : �̵� ���� ������� ���� ��ǥ�� ������
					ChildBar->SetActorRelativeLocation(BarLocation);
					// �� ���ν����� �޽� ����
					ChildBar->CreateMesh(ScaledHeight, CurrentValue);
					// �� �� �ؽ�Ʈ ������ �ʱ�ȭ
					ChildBar->InitializeTextMeshLabel(LabelName);
					// �� �� �ؽ�Ʈ ������ �ʱ�ȭ
					ChildBar->InitializeTextMeshValue(CurrentValue);
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

	return false;
}