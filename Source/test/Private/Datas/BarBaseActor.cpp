// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/TextRenderComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
ABarBaseActor::ABarBaseActor()
{
	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRootComponent;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(RootComponent);

	LegacyActorSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LegacyMeshOffsetLayer"));
	LegacyActorSceneComponent->SetupAttachment(RootComponent);

	LegacyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LegacyMesh"));
	LegacyStaticMeshComponent->SetupAttachment(LegacyActorSceneComponent);

	// Procedural Mesh Component�� Navigation �ý��ۿ��� ����. ���Ž���̳� ai��ȣ�ۿ��� �ʿ� ���� ���, ���� ����. 
	// �Ȳ��� Navigation system���� ȭ��. (��� ������)
	ProcMeshComponent->SetCanEverAffectNavigation(false);
	BarAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BarAnimationTimeline"));

	// �ؽ�Ʈ ������ - ��
	TextRenderComponentValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentValue"));
	TextRenderComponentValue->SetupAttachment(RootComponent);

	// �ؽ�Ʈ �ǹ��� ����� ����
	TextRenderComponentValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	
	// �ؽ�Ʈ ������ - ��
	TextRenderComponentLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentLabel")); 
	TextRenderComponentLabel->SetupAttachment(RootComponent);

	TextRenderComponentLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentLabel->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// Called when the game starts or when spawned
void ABarBaseActor::BeginPlay()
{
	Super::BeginPlay();

}

void ABarBaseActor::CreateProceduralBoxMesh(float BarHeight)
{
	// �ʿ��� �迭 ����
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;

	float BarWidth = UnitSize*Width_bar;
	float w = BarWidth / 2;

	// �ڽ��� ũ�� ���� (�ڽ� �ʺ�)
	FVector BoxRadius(w, w, w); // �ڽ��� ���� ũ��

	// �ڽ� �޽� ����
	UKismetProceduralMeshLibrary::GenerateBoxMesh(BoxRadius, Vertices, Triangles, Normals, UV0, Tangents);

	// �ڽ� ���̸�ŭ ������ ����ؼ� ���� �ø�.
	float zScaler = BarHeight / (UnitSize*BarWidth);
	UE_LOG(LogTemp, Log, TEXT("ABarBaseActor : Calculate ZScaler : zScaler : %f, BarHeight : %f, UnitSIZE : %d, BarWidth : %f"), zScaler, BarHeight, UnitSize, BarWidth);
	//UE_LOG(LogTemp, Log, TEXT("BarBaseActor : zScaler is %f"), zScaler);
	ProcMeshComponent->SetWorldScale3D(FVector(1.f, 1.f, zScaler));
	ProcMeshComponent->AddWorldOffset(FVector(0, 0, (BarHeight / 2)));

	// Procedural Mesh Component�� �޽� ����
	ProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, {}, Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

void ABarBaseActor::CreateLegacyMesh(float BarHeight)
{

}

void ABarBaseActor::CreateMesh(float BarHeight)
{
	if (isProceduralMeshUsing)
	{
		CreateProceduralBoxMesh(BarHeight);
	}
	else
	{
		CreateLegacyMesh(BarHeight);
	}
}

// �� �ؽ�Ʈ ������ ����
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{

	// �ؽ�Ʈ ����
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));

	// �ؽ�Ʈ ���� 
	TextRenderComponentLabel->SetTextRenderColor(TextColor);

}

// �� �ؽ�Ʈ ������ ����
void ABarBaseActor::InitializeTextMeshValue(const float& FloatValue, const float& BarHeight)
{
	int padding = 10;
	
	// �ؽ�Ʈ ����
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));

	// �ؽ�Ʈ ����
	TextRenderComponentValue->SetTextRenderColor(TextColor);

	// �ؽ�Ʈ ũ��
	//TextRenderComponentValue->SetWorldSize(UnitSize*TextSizeUnit_value);

	// ��ġ
	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TextRenderComponentValue->WorldSize)/2 
		+ padding));
}

// �ִϸ��̼� ���� ����
void ABarBaseActor::PlayBarAnimation()
{
	if (AnimationCurve)
	{
		FOnTimelineFloat TimelineCallBack;
		// Ÿ�Ӷ��ο� �Լ� ���ε�
		TimelineCallBack.BindUFunction(this, FName("OnAnimationUpdate"));

		// Timeline�� Curve�� Curve�� ����� Callback �Լ� �߰�
		BarAnimationTimeline->AddInterpFloat(AnimationCurve, TimelineCallBack);
		BarAnimationTimeline->SetLooping(false);
		BarAnimationTimeline->PlayFromStart();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BarBaseActor : Failed finding Animation Cuvrve"));
	}
}

// ���� ������ ���� �ִϸ��̼� ����
void ABarBaseActor::OnAnimationUpdate(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("BarBaseActor : BarAnimation Executing, Current Height : %f"), Value);

	FVector CurrentScale = GetActorScale();
	SetActorScale3D(FVector(CurrentScale.X, CurrentScale.Y, Value));
}



