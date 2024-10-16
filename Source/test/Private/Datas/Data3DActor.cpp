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
#include "Components/TextRenderComponent.h"

/// <summary>
/// BaseActor Functions
/// </summary>

AData3DActor::AData3DActor()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Initializing %s"), *GetName());

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootSceneComponent);

	TextRenderComponent_chartTitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_title"));
	TextRenderComponent_chartTitle->SetupAttachment(RootSceneComponent);
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

AData3DActorBar::AData3DActorBar()
{
	BarGeneratorComponent = CreateDefaultSubobject<UBarGenerator>(TEXT("barGeneratorComponent"));
	if (RootSceneComponent)
	{
		// ó������ ChartGenerator �����ڿ��� ���� ������Ʈ �ٿ�����, ���� ������ ����� �� ������ ���� �ʴ� ���� �߻�.
		// -> ���Ϳ��� ���� ������Ʈ�� ���� ������Ʈ�� ������ RootComponent�� Attach
		BarGeneratorComponent->SetupAttachment(RootSceneComponent);
		BarGeneratorComponent->SplineComponent_height->SetupAttachment(BarGeneratorComponent);
		BarGeneratorComponent->SplineComponent_length->SetupAttachment(BarGeneratorComponent);
		BarGeneratorComponent->ChildActorContainComponent->SetupAttachment(BarGeneratorComponent);
	}
	TextRenderComponent_chartXaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_xAxis"));
	TextRenderComponent_chartXaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartYaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_yAxis"));
	TextRenderComponent_chartYaxisName->SetupAttachment(RootSceneComponent);
}

// ��Ʈ Ÿ��Ʋ �ʱ�ȭ �Լ�
void AData3DActorBar::SetChartLabelText()
{
	if (DataClassInstance)
	{
		UShapeChartClass* TempCastedDataClass = Cast<UShapeChartClass>(DataClassInstance);
		FString ChartTitle = TempCastedDataClass->GetChartTitle();
		TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Chart Title : %s"), *ChartTitle);
	}
}

void AData3DActorBar::SetDataClassInstance()
{
	if (DataManagerReference)
	{
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Getting Last Chart Data Class Instance"));
		DataClassInstance = DataManagerReference->GetChartDataClassInstance(ChartClassNames::NAME_BARCHART);
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
		// ��Ʈ Ÿ��Ʋ �ʱ�ȭ
		SetChartLabelText();
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


