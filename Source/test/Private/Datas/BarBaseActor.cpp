// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ABarBaseActor::ABarBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProcMeshComponent;

	// Procedural Mesh Component를 Navigation 시스템에서 제외. 경로탐색이나 ai상호작용이 필요 없는 경우, 꺼도 좋음. 
	// 안끄면 Navigation system에서 화냄. (계속 감시중)
	ProcMeshComponent->SetCanEverAffectNavigation(false);
	BarAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BarAnimationTimeline"));

	// 에디터상에서도 틱 계속 돌아가게끔. 추후 성능 모니터링 필요.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;
	//PrimaryActorTick.bTickEvenWhenPaused = true;

	// 텍스트 렌더러 - 값
	TextRenderComponentValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentValue"));
	TextRenderComponentValue->SetupAttachment(RootComponent);

	// 텍스트 피벗을 가운데로 설정
	TextRenderComponentValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	// 텍스트의 위치를 부모의 위치로 설정
	//TextRenderComponentValue->SetRelativeLocation(FVector::ZeroVector);
	TextRenderComponentValue->SetRelativeRotation(FRotator(0, 90, 0));

	
	// 텍스트 렌더러 - 라벨
	TextRenderComponentLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentLabel")); 
	TextRenderComponentLabel->SetupAttachment(RootComponent);

	TextRenderComponentLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentLabel->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextRenderComponentLabel->SetRelativeRotation(FRotator(0, 90, 0));
	// 텍스트 렌더러가 메쉬에서 얼마나 떨어져있는지, 일단 하드코딩, 나중에 수정할 것
	//TextRenderComponentLabel->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	//TextRenderComponentLabel->SetRelativeLocation(FVector::ZeroVector);

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
	
	float BarWidth = Width_bar;
	float w = Width_bar / 2;

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
	Triangles.Append({ 0,1,2,0,2,3 });	// 밑면
	Triangles.Append({ 4,7,6,4,6,5 });	// 윗면
	Triangles.Append({ 2,6,7,2,7,3 });	// 앞
	Triangles.Append({ 1,5,6,1,6,2 });	// 오른쪽
	Triangles.Append({ 0,4,5,0,5,1});	// 뒤
	Triangles.Append({ 3,7,4,3,4,0 });	// 왼쪽

	// 노멀 (법선 벡터)
	Normals.Append({ FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) });
	Normals.Append({ FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) });
	Normals.Append({ FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) });
	Normals.Append({ FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) });
	Normals.Append({ FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) });
	Normals.Append({ FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector (- 1, 0, 0), FVector(-1, 0, 0)});


	// UV (트라이앵글)의 각 꼭짓점 부분을 2D 사각형으로 매핑)
	UVs.Add(FVector2D(0.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(0.f, 1.f));
	UVs.Add(FVector2D(0.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(1.f, 1.f));
	UVs.Add(FVector2D(0.f, 1.f));


	// 탄젠트 (법선에 직교하는 벡터 + UV 평면의 U축을 따라가는 벡터)
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));

	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));

	// 프로시저럴 메쉬
	ProcMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

// 라벨 텍스트 렌더러 설정
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{
	int padding = 10;

	// 텍스트 내용
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));

	// 텍스트 색상 
	TextRenderComponentLabel->SetTextRenderColor(TextColor);

	// 텍스트 크기
	TextRenderComponentLabel->SetWorldSize(TextSizeUnit_label);

	// 위치 
	TextRenderComponentLabel->SetRelativeLocation(FVector(0.f, 0.f,-padding));

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
	TextRenderComponentValue->SetWorldSize(TextSizeUnit_value);

	// 위치
	TextRenderComponentValue->SetRelativeLocation(FVector(0.f, 0.f, BarHeight+padding));
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
	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : BarAnimation Executing, Current Height : %f"), Value);

	FVector CurrentScale = GetActorScale();
	SetActorScale3D(FVector(CurrentScale.X, CurrentScale.Y, Value));
}


// Called every frame
//void ABarBaseActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

