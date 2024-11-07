	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCM3DChartActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/JCMCore.h"
#include "Datas/JCMBarBaseActor.h"
#include "Datas/JCMDataManager.h"
#include "Datas/JCMChartGenerator.h"
#include "Datas/JCMDataContainer.h"
#include "Datas/JCMHttpHandler.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"

/// <summary>
/// BaseActor Functions
/// </summary>

AJCM3DChartActor::AJCM3DChartActor()
{
	UE_LOG(LogTemp, Log, TEXT("Data3DActor : Initializing %s"), *GetName());

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));
	TextRenderComponent_chartTitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_title"));
	TextRenderComponent_chartTitle->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartTitle->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartTitle->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// Called when the game starts or when spawned
void AJCM3DChartActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeDataManager();
}

// JsonObjectPtr�� �޴� �Լ�
void AJCM3DChartActor::RequestJsonObject(const FString& URL)
{
	if (!DataManagerInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("JCM3DChartActor : RequestJsonObject() Failed. Initialize JCMCore First"));
		return;
	}
	InitializeRequestManager();
	// Request ��������Ʈ ���ε� �Լ� : DataClass ������� �ʱ�ȭ 
	// ��Ƽĳ��Ʈ ��������Ʈ���ٸ� ��������Ʈ ��ü �� �� û���ؾ� ������, ������ �̱�ĳ��Ʈ. ��ü �� �� ���� ���ص� ��.
	RequestManagerInstance->OnParsedJsonObjectPtrReady.BindUObject(this, &AJCM3DChartActor::SetJsonObject);
	IsDataClassInstanceSet = false;
	RequestManagerInstance->MakeGetRequest(URL, false);
}

// FString�� �޴� �Լ�
void AJCM3DChartActor::RequestJsonString(const FString& URL)
{
	InitializeRequestManager();
	RequestManagerInstance->OnRequestedJsonStringReady.BindUObject(this, &AJCM3DChartActor::SetJsonString);
	IsDataClassInstanceSet = false;
	RequestManagerInstance->MakeGetRequest(URL);
}

void AJCM3DChartActor::InitializeDataManager()
{	
	if (UJCMCore::GetJCMCore())
	{
		DataManagerInstance = UJCMCore::GetJCMCore()->GetJCMDataManager();
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
void AJCM3DChartActor::InitializeRequestManager()
{
	if (!RequestManagerInstance)
	{
		RequestManagerInstance = NewObject<UJCMHttpHandler>();
	}
}

// FJsonObject �޾� UDataClasses*�� ����
void AJCM3DChartActor::SetJsonObject(const TSharedPtr<FJsonObject> JsonData)
{
	if (RequestManagerInstance)
	{
		TSharedPtr<FJsonObject> Data = RequestManagerInstance->GetJsonData();
		FDataInstancePair ResultData = DataManagerInstance->InstancingDataClass(Data);
		UE_LOG(LogTemp, Log, TEXT("Data3DChartActor : DataClass Set, Response Chart Class is : %s"), *ResultData.ClassName);
		DataClassInstance = ResultData.DataInstance;
		if (!DataClassInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Data3DChartActor : Received Data Class is invaid"));
		}
		else
		{
			IsDataClassInstanceSet = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DChartActor : RequestManagerInstance is Invalid"));
	}
}

void AJCM3DChartActor::SetJsonString(const bool IsWorkDone)
{
	IsRequestJsonStringDone = IsWorkDone;
}


// Called every frame
void AJCM3DChartActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AJCM3DChartActorBar::AJCM3DChartActorBar()
{
	BarGeneratorComponent = CreateDefaultSubobject<UJCMChartGeneratorBar>(TEXT("barGeneratorComponent"));
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
void AJCM3DChartActorBar::SetChartDefaultTexts()
{
	if (DataClassInstance)
	{
		UJCMDataContainerBar* TempCastedDataClass = Cast<UJCMDataContainerBar>(DataClassInstance);
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

void AJCM3DChartActorBar::GenerateChartRoutine()
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
	EJCMChartTypes ECurrentChartType = DataClassInstance->GetChartType();
	UE_LOG(LogTemp, Log, TEXT("Data3DActorBar : LastChartType is %d"), ECurrentChartType);

	// �� ������ ��ü�� ������ Ŭ���� ��ü ĳ����
	UJCMDataContainerBar* BarDataClassInstance = Cast<UJCMDataContainerBar>(DataClassInstance);

	// GenerateBarChart() : ������ �Է� �޾� ��Ʈ ���� ��ƾ �Լ� ȣ�� / GetShapeChartData() : Bar(���)��Ʈ ������ Get
	BarGeneratorComponent->GenerateBarChart(BarDataClassInstance->GetShapeChartData(), EnableGenerateMeshAtSplinePoint);

}

