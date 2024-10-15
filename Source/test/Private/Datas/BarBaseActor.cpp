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

	// Procedural Mesh Component를 Navigation 시스템에서 제외. 경로탐색이나 ai상호작용이 필요 없는 경우, 꺼도 좋음. 
	// 안끄면 Navigation system에서 화냄. (계속 감시중)
	ProcMeshComponent->SetCanEverAffectNavigation(false);
	BarAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BarAnimationTimeline"));

	// 텍스트 렌더러 - 값
	TextRenderComponentValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentValue"));
	TextRenderComponentValue->SetupAttachment(RootComponent);

	// 텍스트 피벗을 가운데로 설정
	TextRenderComponentValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	
	// 텍스트 렌더러 - 라벨
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

	// 버텍스
	Vertices.Add(FVector(-w, -w, 0));
	Vertices.Add(FVector(w, -w, 0));
	Vertices.Add(FVector(w, w, 0));
	Vertices.Add(FVector(-w, w, 0));
	Vertices.Add(FVector(-w, -w, BarHeight));
	Vertices.Add(FVector(w, -w, BarHeight));
	Vertices.Add(FVector(w, w, BarHeight));
	Vertices.Add(FVector(-w, w, BarHeight));

	// 트라이앵글
	Triangles.Append({ 4,6,5 });	// 밑면
	Triangles.Append({ 4,7,6 });	

	Triangles.Append({ 0,1,2 });	// 윗면
	Triangles.Append({ 0,2,3 });	

	Triangles.Append({ 0,3,7 });	// 뒤
	Triangles.Append({ 0,7,4 });

	Triangles.Append({ 1,5,6 });	// 앞
	Triangles.Append({ 1,6,2 });	

	Triangles.Append({ 3,2,6 });	// 왼쪽
	Triangles.Append({ 3,6,7 });	

	Triangles.Append({ 0,4,5 });	// 오른쪽
	Triangles.Append({ 0,5,1 });	

	// 노멀 초기화
	Normals.Init(FVector::ZeroVector, Vertices.Num());

	// 각 삼각형 노멀 계산
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

	// 노멀 벡터 정규화
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

	// 탄젠트 설정
	Tangents.Init(FProcMeshTangent(1.f, 0.f, 0.f), Vertices.Num());

	// 프로시저럴 메쉬
	ProcMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

void ABarBaseActor::CreateBoxMesh(float BarHeight)
{
	// 필요한 배열 선언
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;

	float BarWidth = UnitSize*Width_bar;
	float w = BarWidth / 2;

	// 박스의 크기 설정 (박스 너비)
	FVector BoxRadius(w, w, w); // 박스의 절반 크기

	// 박스 메쉬 생성
	UKismetProceduralMeshLibrary::GenerateBoxMesh(BoxRadius, Vertices, Triangles, Normals, UV0, Tangents);

	// 박스 높이만큼 스케일 계산해서 높이 올림.
	float zScaler = BarHeight / (UnitSize*BarWidth);
	UE_LOG(LogTemp, Log, TEXT("ABarBaseActor : Calculate ZScaler : zScaler : %f, BarHeight : %f, UnitSIZE : %d, BarWidth : %f"), zScaler, BarHeight, UnitSize, BarWidth);
	//UE_LOG(LogTemp, Log, TEXT("BarBaseActor : zScaler is %f"), zScaler);
	ProcMeshComponent->SetWorldScale3D(FVector(1.f, 1.f, zScaler));
	ProcMeshComponent->AddWorldOffset(FVector(0, 0, (BarHeight / 2)));

	// Procedural Mesh Component에 메쉬 적용
	ProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, {}, Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

// 라벨 텍스트 렌더러 설정
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{

	// 텍스트 내용
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));

	// 텍스트 색상 
	TextRenderComponentLabel->SetTextRenderColor(TextColor);

	// 텍스트 크기
	//TextRenderComponentLabel->SetWorldSize(UnitSize * TextSizeUnit_label);

	// 위치 
	//TextRenderComponentLabel->SetRelativeLocation(FVector(0.f, 0.f,-1*(UnitSize*TextSizeUnit_label)/ 2));

}

// 값 텍스트 렌더러 설정
void ABarBaseActor::InitializeTextMeshValue(const float& FloatValue, const float& BarHeight)
{
	int padding = 10;
	
	// 텍스트 내용
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));

	// 텍스트 색상
	TextRenderComponentValue->SetTextRenderColor(TextColor);

	// 텍스트 크기
	//TextRenderComponentValue->SetWorldSize(UnitSize*TextSizeUnit_value);

	// 위치
	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TextRenderComponentValue->WorldSize)/2 
		+ padding));
}

// 애니메이션 실행 제어
void ABarBaseActor::PlayBarAnimation()
{
	if (AnimationCurve)
	{
		FOnTimelineFloat TimelineCallBack;
		// 타임라인에 함수 바인딩
		TimelineCallBack.BindUFunction(this, FName("OnAnimationUpdate"));

		// Timeline에 Curve와 Curve를 사용할 Callback 함수 추가
		BarAnimationTimeline->AddInterpFloat(AnimationCurve, TimelineCallBack);
		BarAnimationTimeline->SetLooping(false);
		BarAnimationTimeline->PlayFromStart();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BarBaseActor : Failed finding Animation Cuvrve"));
	}
}

// 막대 스케일 변경 애니메이션 실행
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

