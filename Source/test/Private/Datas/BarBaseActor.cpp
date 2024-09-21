// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ABarBaseActor::ABarBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProcMeshComponent;

	// Procedural Mesh Component�� Navigation �ý��ۿ��� ����. ���Ž���̳� ai��ȣ�ۿ��� �ʿ� ���� ���, ���� ����. 
	// �Ȳ��� Navigation system���� ȭ��. (��� ������)
	ProcMeshComponent->SetCanEverAffectNavigation(false);
	BarAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BarAnimationTimeline"));
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

	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : CreateBarMesh : BarHeight= %f, BarWidth : %f"), BarHeight, BarWidth);

	// ���ؽ�
	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(BarWidth, 0, 0));
	Vertices.Add(FVector(BarWidth, BarWidth, 0));
	Vertices.Add(FVector(0, BarWidth, 0));
	
	Vertices.Add(FVector(0, 0, BarHeight));
	Vertices.Add(FVector(BarWidth, 0, BarHeight));
	Vertices.Add(FVector(BarWidth, BarWidth, BarHeight));
	Vertices.Add(FVector(0, BarWidth, BarHeight));

	// Ʈ���̾ޱ�
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(4);
	Triangles.Add(1); Triangles.Add(5); Triangles.Add(4);

	Triangles.Add(2); Triangles.Add(3); Triangles.Add(7);
	Triangles.Add(2); Triangles.Add(7); Triangles.Add(6);

	Triangles.Add(0); Triangles.Add(4); Triangles.Add(7);
	Triangles.Add(0); Triangles.Add(7); Triangles.Add(3);

	Triangles.Add(1); Triangles.Add(2); Triangles.Add(6);
	Triangles.Add(1); Triangles.Add(6); Triangles.Add(5);

	Triangles.Add(0); Triangles.Add(3); Triangles.Add(2);
	Triangles.Add(0); Triangles.Add(2); Triangles.Add(1);

	Triangles.Add(4); Triangles.Add(5); Triangles.Add(6);
	Triangles.Add(4); Triangles.Add(6); Triangles.Add(7);

	// ���
	Normals.Init(FVector(0, 0, 1), Vertices.Num());
	// UV
	UVs.Init(FVector2d(0, 0), Vertices.Num());
	// ź��Ʈ
	Tangents.Init(FProcMeshTangent(1, 0, 0), Vertices.Num());

	// ���ν����� �޽�
	ProcMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}

	PlayBarAnimation();
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

