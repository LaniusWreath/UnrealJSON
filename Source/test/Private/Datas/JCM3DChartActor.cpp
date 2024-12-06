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
#include "Datas/JCMLog.h"


/// <summary>
/// BaseActor Functions
/// </summary>
// json�� �޾ƿ� DataClass��� ���� ������ �����̳� Ŭ������ �������� �����ϰ�����.
// ���� �� ���� �����ʹ� ���� ���� �߻�Ŭ������ ��������
// �������Ʈ������ ���Ͱ� �ڽ� Ŭ������ ��üȭ�Ǿ������Ƿ�, ������ �����̳� Ŭ������ ȣ�� �� ������ ���� ���Ŀ� �°�
// ĳ�������ִ� ������ �ʿ���. �ٸ� �ڽ� 3DActor Ŭ�������� �������� ��.
// 
// 1205 ���� : �ܺη� ������ get�� set �Լ������� DataContainer��, ���ο����� Ŭ������ �°� ���� �ְԲ� ����
// 1206 ���� : �θ� 3DChartActor�� DataContainer ������, �ڽ� 3DChartActorBar�� DataContainerBar�� �����Բ� �и�
//

AJCM3DChartActor::AJCM3DChartActor()
{
	UE_LOG(JCMlog, Log, TEXT("Data3DActor : Initializing %s"), *GetName());

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
}

// JsonObjectPtr�� �޴� �Լ�
void AJCM3DChartActor::RequestJsonObject(const FString& URL)
{
	SetJCMDataManagerRef();
	InitializeRequestHandler();
	// Request ��������Ʈ ���ε� �Լ� : DataClass ������� �ʱ�ȭ 
	// ��Ƽĳ��Ʈ ��������Ʈ���ٸ� ��������Ʈ ��ü �� �� û���ؾ� ������, ������ �̱�ĳ��Ʈ. ��ü �� �� ���� ���ص� ��.
	RequestHandlerInstance->OnParsedJsonObjectPtrReady.BindUObject(this, &AJCM3DChartActor::CallInstancingDataContainer);
	SetbDataContainerSet(false);
	RequestHandlerInstance->MakeGetRequest(URL, false);
}

// FString�� �޴� �Լ�
void AJCM3DChartActor::RequestJsonString(const FString& URL)
{
	SetJCMDataManagerRef();
	InitializeRequestHandler();
	RequestHandlerInstance->OnRequestedJsonStringReady.BindUObject(this, &AJCM3DChartActor::SetJsonString);
	bDataContainerSet = false;
	RequestHandlerInstance->MakeGetRequest(URL);
}

// ���� Json �о� ������ �����̳� ����
UJCMDataContainer* AJCM3DChartActor::LoadFromLocalJsonFile(const FString& FilePath)
{
	SetJCMDataManagerRef();
	bDataContainerSet = false;
	UJCMDataContainer* NewDataContainer = DataManagerInstanceRef->InstancingDataContainerFromLocalJson(FilePath);
	if (NewDataContainer)
	{
		SetbDataContainerSet(true);
		return NewDataContainer;
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Load from local json failed"), *this->GetActorLabel());
		return nullptr;
	}
}

// �⺻ ���� ���Ἲ üũ �Լ�
bool AJCM3DChartActor::CheckJCMActorIntegrity()
{
	bool bIsValid= true;
	if (!DataManagerInstanceRef)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : DataManager is Invalid "), *this->GetActorLabel());
		bIsValid = false;
	}

	return bIsValid;
}

void AJCM3DChartActor::SetJCMDataManagerRef()
{	
	if (!DataManagerInstanceRef)
	{
		if (UJCMCore::GetJCMCore())
		{
			DataManagerInstanceRef = UJCMCore::GetJCMCore()->GetDataManager();
			if (!DataManagerInstanceRef)
			{
				UE_LOG(JCMlog, Warning, TEXT("%s: Getting DataManager Reference Failed"), *this->GetActorLabel());
			}
		}
		else
		{
			UE_LOG(JCMlog, Warning, TEXT("%s: Couldn't find JCMCore"), *this->GetActorLabel());
		}
	}
}

// RequestManager�� �ν��Ͻ��� ���� request�� ����ϴ� ���Ϳ��� ���� ����. 
// �� �ν��Ͻ��� ��������Ʈ�� �� ������ url�� ���� ������Ʈ�� �����Ͽ� �� ���� �ν��Ͻ��� �Լ��� 1:1 ���ε�
// url�� �����͸� ������Ʈ �� �� ���� ���� httpManager�ν��Ͻ��� �����ϰ� ���Ӱ� �ʱ�ȭ.
const UJCMHttpHandler* AJCM3DChartActor::InitializeRequestHandler()
{
	if (!RequestHandlerInstance)
	{
		RequestHandlerInstance = NewObject<UJCMHttpHandler>();
	}
	return RequestHandlerInstance;
}


// FJsonObject �޾� DataManager���� UDataClasses*�� ���� ��û
void AJCM3DChartActor::CallInstancingDataContainer(const TSharedPtr<FJsonObject> JsonData)
{
	if (!RequestHandlerInstance)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: RequestManagerInstance is Invalid"), *this->GetActorLabel());
		return;
	}

	FDataInstancePair ResultData = DataManagerInstanceRef->InstancingDataContainer(JsonData);
	if (!ResultData.IsValid)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Received source data is invalid "), *this->GetActorLabel());
		return;
	}

	UJCMDataContainer* NewDataContainer = ResultData.DataInstance;
	if (!NewDataContainer)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Data Container instancing failed"), *this->GetActorLabel());
	}

	DataContainer = NewDataContainer;
	SetbDataContainerSet(true);
}

void AJCM3DChartActor::SetJsonString(const bool IsWorkDone)
{
	IsRequestJsonStringDone = IsWorkDone;
}

void AJCM3DChartActor::SetbDataContainerSet(bool InState)
{
	bDataContainerSet = InState;
}

const bool AJCM3DChartActor::GetbDataContainerSet() const
{
	return bDataContainerSet;
}

// Called every frame
void AJCM3DChartActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/// <AJCM3DChartActor>
/// 
/// <AJCM3DChartActorBar>

AJCM3DChartActorBar::AJCM3DChartActorBar()
{
	EnableGenerateMeshAtSplinePoint = false;
	ChartGeneratorComponent = CreateDefaultSubobject<UJCMChartGeneratorBar>(TEXT("BarGeneratorComponent"));
	// ó������ ChartGenerator �����ڿ��� ���� ������Ʈ �ٿ�����, ���� ������ ����� �� ������ ���� �ʴ� ���� �߻�.
	ChartGeneratorComponent->SetupAttachment(RootSceneComponent);
	// ChartGenerator�� ��� ������Ʈ ���� �������ִ� �Լ� �ۼ�
	ChartGeneratorComponent->SetAttachComponents(ChartGeneratorComponent);

	TextRenderComponent_chartXaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_xAxis"));
	TextRenderComponent_chartXaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartXaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartXaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	TextRenderComponent_chartYaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_yAxis"));
	TextRenderComponent_chartYaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartYaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartYaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

const UJCMDataContainerBar* AJCM3DChartActorBar::GetDataContainerRef()
{
	if (!bDataContainerSet)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s :DataContainer not found"), *this->GetActorLabel());
		return nullptr;
	}
	return DataContainerBar;
}

void AJCM3DChartActorBar::SetDataContainer(UJCMDataContainer* DataContainerRef)
{
	UJCMDataContainerBar* NewContainer = CastChecked<UJCMDataContainerBar>(DataContainerRef);
	DataContainerBar = NewContainer;
}

// ��Ʈ Ÿ��Ʋ, X��, Y�� �̸� �ʱ�ȭ �Լ�
void AJCM3DChartActorBar::SetChartDefaultTexts()
{
	if (!DataContainerBar)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : not found DataContainerBar"), *this->GetActorLabel());
		return;
	}

	FString ChartTitle = DataContainerBar->GetChartDataStruct().ChartTitle;
	TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));

	FString ChartXAxisName = DataContainerBar->GetChartDataStruct().XName;
	TextRenderComponent_chartXaxisName->SetText(FText::FromString(ChartXAxisName));

	FString ChartYAxisName = DataContainerBar->GetChartDataStruct().YName;
	TextRenderComponent_chartYaxisName->SetText(FText::FromString(ChartYAxisName));
}

void AJCM3DChartActorBar::GenerateChartRoutine()
{
	if (!ChartGeneratorComponent)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : GenerateChartRoutine : DataContainer is invalid"), *this->GetActorLabel());
		return;
	}

	// ��Ʈ Ÿ��Ʋ �ʱ�ȭ
	SetChartDefaultTexts();

	// ������ �� �ҽ� ���� BarGenerator�� ����
	if (ChartGeneratorComponent && BarBaseActorBPClass)
	{
		ChartGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : BarBaseActorBPClass or BarGeneratorComponent is invalid"), *this->GetActorLabel());
		return;
	}

	// GenerateBarChart() : ������ �Է� �޾� ��Ʈ ���� ��ƾ �Լ� ȣ�� / GetShapeChartData() : Bar(���)��Ʈ ������ Get
	ChartGeneratorComponent->GenerateBarChart(DataContainerBar->GetChartDataStruct(), EnableGenerateMeshAtSplinePoint);
}

// Bar ���� ���Ἲ üũ �Լ�
bool AJCM3DChartActorBar::CheckJCMActorIntegrity()
{
	bool bIsValid = Super::CheckJCMActorIntegrity();

	if (!DataContainerBar)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : DataContainer is Invalid"), *this->GetActorLabel());
		bIsValid = false;
	}

	if (!ChartGeneratorComponent)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : BarGeneratorComponent Invalid"), *this->GetActorLabel());
		bIsValid = false;
	}
	if (!BarBaseActorBPClass)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : BarBaseActorBPClass Invalid"), *this->GetActorLabel());
		bIsValid = false;
	}

	return bIsValid;
}

// DataContainer-> DataContainerBar�� ĳ���� �� ��ġ
void AJCM3DChartActorBar::CallInstancingDataContainer(const TSharedPtr<FJsonObject> JsonData)
{
	if (!RequestHandlerInstance)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: RequestManagerInstance is Invalid"), *this->GetActorLabel());
		return;
	}

	FDataInstancePair ResultData = DataManagerInstanceRef->InstancingDataContainer(JsonData);
	if (!ResultData.IsValid)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Received source data is invalid "), *this->GetActorLabel());
		return;
	}

	UJCMDataContainerBar* NewDataContainer = Cast<UJCMDataContainerBar>(ResultData.DataInstance);
	if (!NewDataContainer)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Data Container instanci failed"), *this->GetActorLabel());
		return;
	}

	DataContainerBar = NewDataContainer;
	SetbDataContainerSet(true);
}

