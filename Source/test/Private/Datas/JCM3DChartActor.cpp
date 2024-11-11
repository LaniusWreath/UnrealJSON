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
}

// JsonObjectPtr�� �޴� �Լ�
void AJCM3DChartActor::RequestJsonObject(const FString& URL)
{
	SetJCMDataManagerRef();
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
	SetJCMDataManagerRef();
	InitializeRequestManager();
	RequestManagerInstance->OnRequestedJsonStringReady.BindUObject(this, &AJCM3DChartActor::SetJsonString);
	IsDataClassInstanceSet = false;
	RequestManagerInstance->MakeGetRequest(URL);
}

// ���� Json �о� ������ �����̳� ����
void AJCM3DChartActor::LoadFromLocalJsonFile(const FString& FilePath)
{
	SetJCMDataManagerRef();
	IsDataClassInstanceSet = false;
	DataClassInstance = DataManagerInstanceRef->InstancingDataContainerFromLocalJson(FilePath);
}

// �⺻ ���� ���Ἲ üũ �Լ�
bool AJCM3DChartActor::CheckJCMActorIntegrity()
{
	bool bIsValid= true;
	if (!DataManagerInstanceRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Integrity Check : %s : DataManager Invalid "), *this->GetName());
		bIsValid = false;
	}
	if (!DataClassInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Integrity Check : %s : DataContainer Invalid"), *this->GetName());
		bIsValid = false;
	}
	if (!RequestManagerInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Integrity Check : %s : HttpHandler Instance Invalid"), *this->GetName());
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
				UE_LOG(LogTemp, Warning, TEXT("Data3DActor : Initialize Managers : Getting DataManager Reference Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't find JCMCore"));
		}
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
		//TSharedPtr<FJsonObject> Data = RequestManagerInstance->GetJsonData();
		FDataInstancePair ResultData = DataManagerInstanceRef->InstancingDataClass(JsonData);
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
	if (BarGeneratorComponent && BarBaseActorBPClass)
	{
		BarGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Data3DActor : BarBaseActorBPClass or BarGeneratorComponent is null"));
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

// Bar ���� ���Ἲ üũ �Լ�
bool AJCM3DChartActorBar::CheckJCMActorIntegrity()
{
	bool bIsValid = Super::CheckJCMActorIntegrity();

	if (!BarGeneratorComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Integrity Check : %s : BarGeneratorComponent Invalid"), *this->GetName());
		bIsValid = false;
	}
	if (!BarBaseActorBPClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Integrity Check : %s : BarBaseActorBPClass Invalid"), *this->GetName());
		bIsValid = false;
	}

	return bIsValid;
}