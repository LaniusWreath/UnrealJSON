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
	TextRenderComponent_chartTitle->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartTitle->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
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
		// 처음에는 ChartGenerator 생성자에서 각각 컴포넌트 붙였으나, 계층 구조만 변경될 뿐 실제로 붙지 않는 문제 발생.
		// -> 액터에서 직접 컴포넌트의 구성 컴포넌트들 액터의 RootComponent에 Attach
		BarGeneratorComponent->SetupAttachment(RootSceneComponent);
		BarGeneratorComponent->SplineComponent_height->SetupAttachment(BarGeneratorComponent);
		BarGeneratorComponent->SplineComponent_length->SetupAttachment(BarGeneratorComponent);
		BarGeneratorComponent->ChildActorContainComponent->SetupAttachment(BarGeneratorComponent);
	}
	TextRenderComponent_chartXaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_xAxis"));
	TextRenderComponent_chartXaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartXaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartXaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	TextRenderComponent_chartYaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_yAxis"));
	TextRenderComponent_chartYaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartYaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartYaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// 차트 타이틀, X축, Y축 이름 초기화 함수
void AData3DActorBar::SetChartDefaultTexts()
{
	if (DataClassInstance)
	{
		UShapeChartClass* TempCastedDataClass = Cast<UShapeChartClass>(DataClassInstance);
		FString ChartTitle = TempCastedDataClass->GetChartTitle();
		TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Chart Title : %s"), *ChartTitle);

		FString ChartXAxisName = TempCastedDataClass->GetChartXName();
		TextRenderComponent_chartXaxisName->SetText(FText::FromString(ChartXAxisName));

		FString ChartYAxisName = TempCastedDataClass->GetChartYName();
		TextRenderComponent_chartYaxisName->SetText(FText::FromString(ChartYAxisName));
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
		// 차트 타이틀 초기화
		SetChartDefaultTexts();
	}

	// 생성할 바 소스 액터 BarGenerator에 전달
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
		// 데이터로부터 차트 타입 추출
		EChartTypes ECurrentChartType = DataClassInstance->GetChartType();
		UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : LastChartType is %d"), ECurrentChartType);

		// 바 데이터 객체로 데이터 클래스 객체 캐스팅
		UShapeChartBarClass* BarDataClassInstance = Cast<UShapeChartBarClass>(DataClassInstance);

		// GenerateBarChart() : 데이터 입력 받아 차트 생성 루틴 함수 호출 / GetShapeChartData() : Bar(모양)차트 데이터 Get
		BarGeneratorComponent->GenerateBarChart(BarDataClassInstance->GetShapeChartData());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActorBar : DataClassInstance is null"));
	}

}


