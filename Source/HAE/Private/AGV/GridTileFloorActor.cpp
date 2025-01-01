// Fill out your copyright notice in the Description page of Project Settings.


#include "AGV/GridTileFloorActor.h"
#include "AGV/AGVLog.h"

// Sets default values
AGridTileFloorActor::AGridTileFloorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // ����ƽ �޽� ������Ʈ �ʱ�ȭ
    GridMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridMesh"));
    GridMesh->SetupAttachment(RootComponent);

    // �⺻ ����ƽ �޽� ����
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Plane"));
    if (MeshAsset.Succeeded()) 
    {
        GridMesh->SetStaticMesh(MeshAsset.Object);
    }
}

// Called when the game starts or when spawned
void AGridTileFloorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

#if WITH_EDITOR
void AGridTileFloorActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // �����Ϳ��� ���͸� ��ġ�� �� �׸��� ����
    GenerateGrid(NumRows, NumColumns, TileSize);
    UpdateGridMesh(NumRows, NumColumns, TileSize);
    UpdateTileVisuals(NumRows, NumColumns);
}

// ���� �Ӽ� ���� �� 
void AGridTileFloorActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // �Ӽ��� ����� �� �׸��� �����
    GenerateGrid(NumRows, NumColumns, TileSize);
    UpdateGridMesh(NumRows, NumColumns, TileSize);
    UpdateTileVisuals(NumRows, NumColumns);
}
#endif

void AGridTileFloorActor::UpdateGridMesh(const int32 InNumRows, const int32 InNumColumns, const float InTileSize )
{
    FVector GridScale = FVector(InNumRows * InTileSize / 100.0f, InNumColumns * InTileSize / 100.0f, 1.0f);
    GridMesh->SetWorldScale3D(GridScale);

    FVector GridLocation = FVector((NumRows * InTileSize) / 2.0f, (InNumColumns * InTileSize) / 2.0f, 0.0f);
    GridMesh->SetWorldLocation(GridLocation);
}

void AGridTileFloorActor::UpdateTileVisuals(int32 Row, int32 Column)
{
    //int32 TileIndex = Row * NumColumns + Column;
    //if (!Tiles.Contains(TileIndex))
    //{
    //    // Tile�� ���� ������ ���� ���� ��Ƽ���� ������Ʈ
    //    if (Tiles[TileIndex].bIsOccupied)
    //    {
    //        // Occupied ���� �ݿ�
    //    }
    //    else
    //    {
    //        // Accessible ���� �ݿ�
    //    }
    //}
}

// Called every frame
void AGridTileFloorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// �׸��� Ÿ�� ����
void AGridTileFloorActor::GenerateGrid(int32 InRows, int32 InColumns, float InTileSize)
{
    // ���� Ÿ�� ����
    Tiles.Empty();

    NumRows = InRows;
    NumColumns = InColumns;
    TileSize = InTileSize;

    for (int32 Row = 0; Row < NumRows; Row++)
    {
        for (int32 Column = 0; Column < NumColumns; Column++)
        {
            FTile NewTile;
            NewTile.bIsAccessible = true;
            NewTile.bIsOccupied = false;

            Tiles.Emplace(FIntPoint(Row, Column), NewTile);
        }
    }
}

// Ư�� ��ǥ�� Ÿ�� ������ Get
FTile* AGridTileFloorActor::GetTileAt(const int32 InRow, const int32 InColumn)
{
    FIntPoint TileKey(InRow, InColumn);
    if(!Tiles.Contains(TileKey))
    {
        UE_LOG(AGVlog, Warning, TEXT("GetTileAt : %d, %d Tile is invalid"), InRow, InColumn);
        return nullptr;
    }

    return &Tiles[TileKey];
}

// Ÿ���� ���� ��ǥ Get
FIntPoint AGridTileFloorActor::GetWorldLocationFromTile(const FVector& InWorldLocation, float InTileSize)
{
    int32 Row = FMath::FloorToInt(InWorldLocation.X / TileSize);
    int32 Column = FMath::FloorToInt(InWorldLocation.Y / TileSize);
    return FIntPoint(Row, Column);
}


