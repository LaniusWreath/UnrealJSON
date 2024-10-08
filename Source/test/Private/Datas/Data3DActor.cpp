	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Data3DActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/DataManageGameInstance.h"
#include "Datas/BarBaseActor.h"
#include "Datas/DataManager.h"
#include "Datas/ChartGenerator.h"
#include "Datas/DataClasses.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"

/// <summary>
/// BaseActor Functions
/// </summary>

AData3DActor::AData3DActor()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Initializing %s"), *GetName());

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AData3DActor::BeginPlay()
{
	Super::BeginPlay();

	InitilizeDataManager();
}

void AData3DActor::InitilizeDataManager()
{
	UDataManageGameInstance* GameInstance = Cast<UDataManageGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance && GameInstance->GetDataManager())
	{
		DataManagerReference = GameInstance->GetDataManager();
		if (DataManagerReference)
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

// �ִϸ��̼� ������ ��
//void AData3DActor::PlayLastChildrenAnimation()
//{
//	if (DataManagerPtr)
//	{
//		UDataClasses* BaseClass = DataManagerPtr->GetLastChartDataClassInstancePtr();
//		EChartTypes ECurrentChartType = BaseClass->GetChartType();
//		UE_LOG(LogTemp, Log, TEXT("Data3DActor : LastChartType is %d"), ECurrentChartType);
//
//		switch (ECurrentChartType)
//		{
//		case None:
//			break;
//		case BAR:
//		{
//			for (const UChildActorComponent* ChildActorComponent : ChildActorComponents)
//			{
//				if (ChildActorComponent && ChildActorComponent->GetChildActor())
//				{
//					ABarBaseActor* BarChildActor = Cast<ABarBaseActor>(ChildActorComponent->GetChildActor());
//					BarChildActor->PlayBarAnimation();
//				}
//			}
//			break;
//		}
//			
//		case LINE:
//		{
//			break;
//		}
//
//		case PIE:
//		{
//			break;
//		}
//		case XY:
//		{
//			break;
//		}
//		case XYZ:
//		{
//			break;
//		}
//		case FREE:
//		{
//			break;
//		}
//		default:
//		{
//			break;
//		}
//		}
//	}
//}

//void AData3DActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//	// �Ӽ� �̸��� "ComponentClass"�� ��쿡�� ó��
//	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
//	if (PropertyName == GET_MEMBER_NAME_CHECKED(AData3DActor, ChartGeneratorComponentClass))
//	{
//		if (ChartGeneratorComponent)
//		{
//			ChartGeneratorComponent->DestroyComponent();  // ���� �θ� ������Ʈ ����
//			ChartGeneratorComponent = nullptr;  // ������ �ʱ�ȭ
//		}
//
//		// ���õ� �ڽ� Ŭ������ ���ο� ������Ʈ ����
//		if (ChartGeneratorComponentClass && ChartGeneratorComponentClass->IsChildOf(UChartGenerator::StaticClass()))
//		{
//			ChartGeneratorComponent = NewObject<UChartGenerator>(this, ChartGeneratorComponentClass, TEXT("ParentComponent"));
//			
//			UBarGenerator* BarGenerateComponent = Cast<UBarGenerator>(ChartGeneratorComponent);
//			BarGenerateComponent->SplineComponent
//
//			ChartGeneratorComponent->SetupAttachment(RootComponent);
//			ChartGeneratorComponent->RegisterComponent();
//
//			// �ڽ� Ŭ������ �����ڰ� ȣ��Ǹ� �ʿ��� �ʱ�ȭ(ȭ��ǥ ������Ʈ �ʱ�ȭ ��)�� �����
//		}
//
//	}
//}


/// <summary>
/// BarActor Funcions
/// </summary>

AData3DActorBar::AData3DActorBar()
{
	BarGeneratorComponent = CreateDefaultSubobject<UBarGenerator>(TEXT("barGeneratorComponent"));
	BarGeneratorComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<ABarBaseActor> ActorClassFinder(TEXT("Content/Data/BP_BarBaseActor01.uasset"));
	if (ActorClassFinder.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("Default Bar Source Blueprint set "));
		BarBaseActorBPClass = ActorClassFinder.Class;
	}
}

void AData3DActorBar::SetDataClassInstance()
{
	if (DataManagerReference)
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Getting Last Chart Data Class Instance"));
		DataClassInstance = DataManagerReference->GetChartDataClassInstance(TEXT("ShapeChartBarClass"));
		if (DataClassInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : DataClassInstance : %s"), *DataClassInstance->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Data3DActor : DataClassInstacning Failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Data3DActor : no DataManagerReference"));
	}
}

void AData3DActorBar::GenerateChartRoutine()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : Generate Chart Routine Running"));
	if (!DataClassInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActorBar : DataClassInstance is null, SetDataClassInstance"));
		SetDataClassInstance();
	}

	// ������ �� �ҽ� ���� BarGenerator�� ����
	if (BarBaseActorBPClass)
	{
		BarGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActor : BarBaseActorBPClass is null"));
	}

	if (DataClassInstance)
	{
		// �����ͷκ��� ��Ʈ Ÿ�� ����
		EChartTypes ECurrentChartType = DataClassInstance->GetChartType();
		UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : LastChartType is %d"), ECurrentChartType);

		// �� ������ ��ü�� ������ Ŭ���� ��ü ĳ����
		UShapeChartBarClass* BarDataClassInstance = Cast<UShapeChartBarClass>(DataClassInstance);

		// GenerateBarChart() : ������ �Է� �޾� ��Ʈ ���� ��ƾ �Լ� ȣ�� / GetShapeChartData() : Bar(���)��Ʈ ������ Get
		BarGeneratorComponent->GenerateBarChart(BarDataClassInstance->GetShapeChartData());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActorBar : DataClassInstance is null"));
	}

}


