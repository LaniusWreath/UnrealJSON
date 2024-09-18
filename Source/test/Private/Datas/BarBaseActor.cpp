// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"


// Sets default values
ABarBaseActor::ABarBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProcMeshComponent;

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

	// ¹öÅØ½º
	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(BarWidth, 0, 0));
	Vertices.Add(FVector(BarWidth, BarWidth, 0));
	Vertices.Add(FVector(0, BarWidth, 0));
	
	Vertices.Add(FVector(0, 0, BarHeight));
	Vertices.Add(FVector(BarWidth, 0, BarHeight));
	Vertices.Add(FVector(BarWidth, BarWidth, BarHeight));
	Vertices.Add(FVector(0, BarWidth, BarHeight));

	// Æ®¶óÀÌ¾Þ±Û
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

	// ³ë¸Ö
	Normals.Init(FVector(0, 0, 1), Vertices.Num());
	// UV
	UVs.Init(FVector2d(0, 0), Vertices.Num());
	// ÅºÁ¨Æ®
	Tangents.Init(FProcMeshTangent(1, 0, 0), Vertices.Num());

	// ÇÁ·Î½ÃÀú·² ¸Þ½¬
	ProcMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
}



// Called every frame
//void ABarBaseActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

