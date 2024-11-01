	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/Data3DActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/JBCMCore.h"
#include "Datas/BarBaseActor.h"
#include "Datas/DataManager.h"
#include "Datas/ChartGenerator.h"
#include "Datas/DataClasses.h"
#include "Datas/HTTPRequestManager.h"
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

	TextRenderComponent_chartTitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_title"));
	TextRenderComponent_chartTitle->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartTitle->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartTitle->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// Called when the game starts or when spawned
void AData3DActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeDataManager();
}

void AData3DActor::CallJsonObject(const FString& URL)
{
	InitializeRequestManager();
	IsDataClassInstanceSet = false;
	RequestManagerInstance->MakeGetRequest(URL);
}

void AData3DActor::InitializeDataManager()
{	
	UJBCMCore* JBCMCoreReference = UJBCMCore::GetJBCMCore();

	if (JBCMCoreReference)
	{
		DataManagerInstance = JBCMCoreReference->GetDataManager();
		if (!DataManagerInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Data3DActor : Initialize Managers : Getting DataManager Reference Failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find GameInstance or DataManager"));
	}
}

// RequestManager�� �ν��Ͻ��� ���� request�� ����ϴ� ���Ϳ��� ���� ����. 
// �� �ν��Ͻ��� ��������Ʈ�� �� ������ url�� ���� ������Ʈ�� �����Ͽ� �� ���� �ν��Ͻ��� �Լ��� 1:1 ���ε�
// url�� �����͸� ������Ʈ �� �� ���� ���� httpManager�ν��Ͻ��� �����ϰ� ���Ӱ� �ʱ�ȭ.
void AData3DActor::InitializeRequestManager()
{
	if (RequestManagerInstance)
	{
		RequestManagerInstance = nullptr;
	}
	RequestManagerInstance = NewObject<UHTTPRequestManager>();

	// Request ��������Ʈ ���ε� �Լ� : DataClass ������� �ʱ�ȭ
	RequestManagerInstance->OnParsedDataReady.AddUObject(this, &AData3DActor::SetJsonObject);
}

// FJsonObject �޾� UDataClasses*�� ����
void AData3DActor::SetJsonObject(const TSharedPtr<FJsonObject> JsonData)
{
	if (RequestManagerInstance)
	{
		TSharedPtr<FJsonObject> Data = RequestManagerInstance->GetJsonData();
		FDataInstancePair ResultData = DataManagerInstance->InstancingDataClass(Data);
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : DataClass Set, Response Chart Class is : %s"), *ResultData.ClassName);
		DataClassInstance = ResultData.DataInstance;
		if (!DataClassInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DActor : Received Data Class is invaid"));
		}
		else
		{
			IsDataClassInstanceSet = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActor : RequestManagerInstance is Invalid"));
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
	TextRenderComponent_chartXaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartXaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	TextRenderComponent_chartYaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_yAxis"));
	TextRenderComponent_chartYaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartYaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartYaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// ��Ʈ Ÿ��Ʋ, X��, Y�� �̸� �ʱ�ȭ �Լ�
void AData3DActorBar::SetChartDefaultTexts()
{
	if (DataClassInstance)
	{
		UShapeChartClass* TempCastedDataClass = Cast<UShapeChartClass>(DataClassInstance);
		//FString ChartTitle = TempCastedDataClass->GetChartTitle();
		FString ChartTitle = TempCastedDataClass->GetShapeChartData().ChartTitle;

		TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));
		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Chart Title : %s"), *ChartTitle);

		FString ChartXAxisName = TempCastedDataClass->GetShapeChartData().XName;
		TextRenderComponent_chartXaxisName->SetText(FText::FromString(ChartXAxisName));

		FString ChartYAxisName = TempCastedDataClass->GetShapeChartData().YName;
		TextRenderComponent_chartYaxisName->SetText(FText::FromString(ChartYAxisName));
	}
}

//UDataClasses* AData3DActorBar::SetDataClassInstance()
//{
//	if (DataManagerInstance)
//	{
//		UE_LOG(LogTemp, Log, TEXT("Data3DActor : Getting Last Chart Data Class Instance"));
//		DataClassInstance = DataManagerInstance->GetChartDataClassInstance(ChartClassNames::NAME_BARCHART);
//		if (DataClassInstance)
//		{
//			UE_LOG(LogTemp, Log, TEXT("Data3DActor : DataClassInstance : %s"), *DataClassInstance->GetName());
//			return DataClassInstance;
//		}
//		else
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Data3DActor : DataClassInstacning Failed"));
//			return nullptr;
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("Data3DActor : no DataManagerReference"));
//		return nullptr;
//	}
//}

void AData3DActorBar::GenerateChartRoutine()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : Generate Chart Routine Running"));
	if (!DataClassInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActorBar : GenerateChartRoutine : DataClassInstance is invalid"));
		return;
	}

	// ��Ʈ Ÿ��Ʋ �ʱ�ȭ
	SetChartDefaultTexts();

	// ������ �� �ҽ� ���� BarGenerator�� ����
	if (BarBaseActorBPClass)
	{
		BarGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActor : BarBaseActorBPClass is null"));
		return;
	}

	// �����ͷκ��� ��Ʈ Ÿ�� ����
	EChartTypes ECurrentChartType = DataClassInstance->GetChartType();
	UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : LastChartType is %d"), ECurrentChartType);

	// �� ������ ��ü�� ������ Ŭ���� ��ü ĳ����
	UShapeChartBarClass* BarDataClassInstance = Cast<UShapeChartBarClass>(DataClassInstance);

	// GenerateBarChart() : ������ �Է� �޾� ��Ʈ ���� ��ƾ �Լ� ȣ�� / GetShapeChartData() : Bar(���)��Ʈ ������ Get
	BarGeneratorComponent->GenerateBarChart(BarDataClassInstance->GetShapeChartData(), EnableGenerateMeshAtSplinePoint);

}

