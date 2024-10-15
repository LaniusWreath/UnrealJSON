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

void ABarBaseActor::CreateBarMesh(float BarHeight)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	
	float BarWidth = Width_bar * UnitSize;
	float w = (BarWidth / 2);

	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : CreateBarMesh : BarHeight= %f, BarWidth : %f"), BarHeight, BarWidth);

	// ���ؽ�
	Vertices.Add(FVector(-w, -w, 0));
	Vertices.Add(FVector(w, -w, 0));
	Vertices.Add(FVector(w, w, 0));
	Vertices.Add(FVector(-w, w, 0));
	Vertices.Add(FVector(-w, -w, BarHeight));
	Vertices.Add(FVector(w, -w, BarHeight));
	Vertices.Add(FVector(w, w, BarHeight));
	Vertices.Add(FVector(-w, w, BarHeight));

	// Ʈ���̾ޱ�
	Triangles.Append({ 4,6,5 });	// �ظ�
	Triangles.Append({ 4,7,6 });	

	Triangles.Append({ 0,1,2 });	// ����
	Triangles.Append({ 0,2,3 });	

	Triangles.Append({ 0,3,7 });	// ��
	Triangles.Append({ 0,7,4 });

	Triangles.Append({ 1,5,6 });	// ��
	Triangles.Append({ 1,6,2 });	

	Triangles.Append({ 3,2,6 });	// ����
	Triangles.Append({ 3,6,7 });	

	Triangles.Append({ 0,4,5 });	// ������
	Triangles.Append({ 0,5,1 });	

	// ��� �ʱ�ȭ
	Normals.Init(FVector::ZeroVector, Vertices.Num());

	// �� �ﰢ�� ��� ���
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		int32 Index0 = Triangles[i];
		int32 Index1 = Triangles[i + 1];
		int32 Index2 = Triangles[i + 2];

		FVector Vertex0 = Vertices[Index0];
		FVector Vertex1 = Vertices[Index1];
		FVector Vertex2 = Vertices[Index2];

		FVector Edge1 = Vertex1 - Vertex0;
		FVector Edge2 = Vertex2 - Vertex0;

		FVector Normal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

		Normals[Index0] += Normal;
		Normals[Index1] += Normal;
		Normals[Index2] += Normal;
	}

	// ��� ���� ����ȭ
	for (FVector& Normal : Normals)
	{
		Normal.Normalize();
	}

	// UV0
	TArray<FVector2D> UV0;
	UV0.Init(FVector2D::ZeroVector, Vertices.Num());

	UV0[0] = FVector2D(0.f, 0.f);
	UV0[1] = FVector2D(1.f, 0.f);
	UV0[2] = FVector2D(1.f, 1.f);
	UV0[3] = FVector2D(0.f, 1.f);

	// UV1
	TArray<FVector2D> UV1;
	UV1.Init(FVector2D::ZeroVector, Vertices.Num());

	UV1[0] = FVector2D(0.f, 0.f);
	UV1[1] = FVector2D(0.25f, 0.f);
	UV1[2] = FVector2D(0.25f, 0.25f);
	UV1[3] = FVector2D(0.f, 0.25f);

	/*TArray<TArray<FVector2D>> UVs;
	for (int i = 0; i < 4; i++)
	{
		UVs.Add(UV0[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		UVs.Add(UV1[i]);
	}*/

	// ź��Ʈ ����
	Tangents.Init(FProcMeshTangent(1.f, 0.f, 0.f), Vertices.Num());

	// ���ν����� �޽�
	ProcMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

void ABarBaseActor::CreateBoxMesh(float BarHeight)
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

// �� �ؽ�Ʈ ������ ����
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{

	// �ؽ�Ʈ ����
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));

	// �ؽ�Ʈ ���� 
	TextRenderComponentLabel->SetTextRenderColor(TextColor);

	// �ؽ�Ʈ ũ��
	//TextRenderComponentLabel->SetWorldSize(UnitSize * TextSizeUnit_label);

	// ��ġ 
	//TextRenderComponentLabel->SetRelativeLocation(FVector(0.f, 0.f,-1*(UnitSize*TextSizeUnit_label)/ 2));

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


// Called every frame
//void ABarBaseActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

