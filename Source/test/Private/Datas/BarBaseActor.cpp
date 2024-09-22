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
	Triangles.Append({ 0,1,2,0,2,3 });	// �ظ�
	Triangles.Append({ 4,7,6,4,6,5 });	// ����
	Triangles.Append({ 2,6,7,2,7,3 });	// ��
	Triangles.Append({ 1,5,6,1,6,2 });	// ������
	Triangles.Append({ 0,4,5,0,5,1});	// ��
	Triangles.Append({ 3,7,4,3,4,0 });	// ����

	// ��� (���� ����)
	Normals.Append({ FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) });
	Normals.Append({ FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) });
	Normals.Append({ FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) });
	Normals.Append({ FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) });
	Normals.Append({ FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) });
	Normals.Append({ FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector (- 1, 0, 0), FVector(-1, 0, 0)});


	// UV (Ʈ���̾ޱ�)�� �� ������ �κ��� 2D �簢������ ����)
	UVs.Add(FVector2D(0.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(0.f, 1.f));
	UVs.Add(FVector2D(0.f, 0.f));
	UVs.Add(FVector2D(1.f, 0.f));
	UVs.Add(FVector2D(1.f, 1.f));
	UVs.Add(FVector2D(0.f, 1.f));


	// ź��Ʈ (������ �����ϴ� ���� + UV ����� U���� ���󰡴� ����)
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));

	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));
	Tangents.Add(FProcMeshTangent(1, 0, 0));

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

