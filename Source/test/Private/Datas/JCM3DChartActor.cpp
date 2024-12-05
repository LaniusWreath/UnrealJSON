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
	RequestHandlerInstance->OnJsonObjectReceivedDelegate_JCM.BindUObject(this, &AJCM3DChartActor::CallInstancingDataContainer);
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
const UJCMDataContainer* AJCM3DChartActor::CallInstancingDataContainer(const TSharedPtr<FJsonObject> JsonData)
{
	if (RequestHandlerInstance)
	{
		FDataInstancePair ResultData = DataManagerInstanceRef->InstancingDataContainer(JsonData);
		if (!ResultData.IsValid)
		{
			UE_LOG(JCMlog, Error, TEXT("%s: Received source data is invalid "), *this->GetActorLabel());
			return nullptr;
		}

		UJCMDataContainer* NewDataContainer = ResultData.DataInstance;
		if (!NewDataContainer)
		{
			UE_LOG(JCMlog, Log, TEXT("%s: Data Container instancing failed"), *this->GetActorLabel());
		}

		SetbDataContainerSet(true);
		return NewDataContainer;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s: RequestManagerInstance is Invalid"), *this->GetActorLabel());
		return nullptr;
	}
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

AJCM3DChartActorBar::AJCM3DChartActorBar()
{
	EnableGenerateMeshAtSplinePoint = false;
	BarGeneratorComponent = CreateDefaultSubobject<UJCMChartGeneratorBar>(TEXT("BarGeneratorComponent"));
	// ó������ ChartGenerator �����ڿ��� ���� ������Ʈ �ٿ�����, ���� ������ ����� �� ������ ���� �ʴ� ���� �߻�.
	BarGeneratorComponent->SetupAttachment(RootSceneComponent);
	// ChartGenerator�� ��� ������Ʈ ���� �������ִ� �Լ� �ۼ�
	BarGeneratorComponent->SetAttachComponents(BarGeneratorComponent);

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
	if (bDataContainerSet)
	{
		UJCMDataContainerBar* Container = Cast<UJCMDataContainerBar>(DataContainerInstance);
		return Container;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : Getting DataContainer Ref Failed"), *this->GetActorLabel());
		return nullptr;
	}
}

void AJCM3DChartActorBar::SetDataContainer(UJCMDataContainer* DataContainerRef)
{
	UJCMDataContainerBar* NewContainer = CastChecked<UJCMDataContainerBar>(DataContainerRef);
	DataContainerInstance = NewContainer;
}

// ��Ʈ Ÿ��Ʋ, X��, Y�� �̸� �ʱ�ȭ �Լ�
void AJCM3DChartActorBar::SetChartDefaultTexts()
{
	if (DataContainerInstance)
	{
		UJCMDataContainerBar* TempCastedDataClass = Cast<UJCMDataContainerBar>(DataContainerInstance);
		FString ChartTitle = TempCastedDataClass->GetChartDataStruct().ChartTitle;

		TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));

		FString ChartXAxisName = TempCastedDataClass->GetChartDataStruct().XName;
		TextRenderComponent_chartXaxisName->SetText(FText::FromString(ChartXAxisName));

		FString ChartYAxisName = TempCastedDataClass->GetChartDataStruct().YName;
		TextRenderComponent_chartYaxisName->SetText(FText::FromString(ChartYAxisName));
	}
}

void AJCM3DChartActorBar::GenerateChartRoutine()
{
	if (!DataContainerInstance)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : GenerateChartRoutine : DataContainer is invalid"), *this->GetActorLabel());
		return;
	}

	// ��Ʈ Ÿ��Ʋ �ʱ�ȭ
	SetChartDefaultTexts();

	// ������ �� �ҽ� ���� BarGenerator�� ����
	if (BarGeneratorComponent && BarBaseActorBPClass)
	{
		BarGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : BarBaseActorBPClass or BarGeneratorComponent is invalid"), *this->GetActorLabel());
		return;
	}

	// �����ͷκ��� ��Ʈ Ÿ�� ����
	EJCMChartTypes ECurrentChartType = DataContainerInstance->GetChartType();

	// �� ������ ��ü�� ������ Ŭ���� ��ü ĳ����
	UJCMDataContainerBar* BarDataClassInstance = Cast<UJCMDataContainerBar>(DataContainerInstance);

	// GenerateBarChart() : ������ �Է� �޾� ��Ʈ ���� ��ƾ �Լ� ȣ�� / GetShapeChartData() : Bar(���)��Ʈ ������ Get
	BarGeneratorComponent->GenerateBarChart(BarDataClassInstance->GetChartDataStruct(), EnableGenerateMeshAtSplinePoint);
}

// Bar ���� ���Ἲ üũ �Լ�
bool AJCM3DChartActorBar::CheckJCMActorIntegrity()
{
	bool bIsValid = Super::CheckJCMActorIntegrity();

	if (!DataContainerInstance)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : DataContainer is Invalid"), *this->GetActorLabel());
		bIsValid = false;
	}

	if (!BarGeneratorComponent)
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
