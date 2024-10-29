// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/BarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/TextRenderComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BodyInstance.h"
#include "TimerManager.h"

// Sets default values
ABarBaseActor::ABarBaseActor()
{
	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRootComponent;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(RootComponent);

	CustomActorSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LegacyMeshOffsetLayer"));
	CustomActorSceneComponent->SetupAttachment(RootComponent);

	CustomStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LegacyMesh"));
	CustomStaticMeshComponent->SetupAttachment(CustomActorSceneComponent);

	CustomStaticMeshComponent->SetHiddenInGame(true);

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

	// ���ø����� �� Ŀ���� ����ƽ �޽��� �ݸ��� ����
	CustomStaticMeshComponent->SetSimulatePhysics(false);
	CustomStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

// ��ü Ŀ���� ����ƽ �޽� ���� ��ƾ
void ABarBaseActor::CreateCustomMeshRoutine(float BarHeight)
{
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshComponent->GetStaticMesh()).Z * 
		CustomStaticMeshComponent->GetRelativeScale3D().Z;

	int32 UnitMeshAmount = BarHeight / UnitMeshHeight;
	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Amount : %d, UnitSize : %f"), UnitMeshAmount, UnitMeshHeight);
	float UnitMeshLeft = BarHeight / UnitMeshHeight - UnitMeshAmount;

	// Ÿ�̸� ����, 0.5�� ���� �ϵ��ڵ�, ���ٷ� �Ű����� �ִ� �Լ� ĸ��
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
	{
		CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount); 
	}, 0.5f, true);

}

// Ŀ���� ����ƽ �޽� ���� �Լ�
void ABarBaseActor::CreateSingleCustomMeshComponent(float BarHeight, float UnitMeshHeight, int32 SpawnAmount)
{
	// StaticMeshComponent�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
	UStaticMeshComponent* UnitMeshComponent = NewObject<UStaticMeshComponent>(this);

	// ���ø��� �Ӽ��� UnitMeshComponent�� ����
	UnitMeshComponent->SetStaticMesh(CustomStaticMeshComponent->GetStaticMesh());
	UnitMeshComponent->SetMaterial(0, CustomStaticMeshComponent->GetMaterial(0));
	UnitMeshComponent->SetRelativeScale3D(CustomStaticMeshComponent->GetRelativeScale3D());

	// ������ ���� : BodyInstance�� ���� ������ �� ������, �� ��쿡�� AttachToComponent�� ����� �� ����. -> ���� ���� �ʿ�
	UnitMeshComponent->SetSimulatePhysics(true); // ������ on
	UnitMeshComponent->SetEnableGravity(true); // �߷� on
	UnitMeshComponent->SetLinearDamping(CustomStaticMeshComponent->GetLinearDamping()); // ���� ����
	UnitMeshComponent->SetAngularDamping(CustomStaticMeshComponent->GetAngularDamping()); // ȸ���� ����
	//UnitMeshComponent->SetPhysMaterialOverride(CustomStaticMeshComponent->GetBodySetup()->GetPhysMaterial()); // ������ ��Ƽ����
	UnitMeshComponent->BodyInstance.SetMassOverride(CustomStaticMeshComponent->BodyInstance.GetMassOverride()); //����

	// �ݸ��� ���� : �ݸ���-�������� bodyinstance�� ���� ���� ��. bodyinstance ��� ���ϴ� ����, �ݸ����� ������ ����
	UnitMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	UnitMeshComponent->SetCollisionObjectType(CustomStaticMeshComponent->GetCollisionObjectType());
	UnitMeshComponent->SetCollisionResponseToChannels(CustomStaticMeshComponent->GetCollisionResponseToChannels());

	// ����Ʈ����Ʈ ����
	UnitMeshComponent->BodyInstance.bLockRotation = CustomStaticMeshComponent->BodyInstance.bLockRotation;
	UnitMeshComponent->BodyInstance.bLockTranslation = CustomStaticMeshComponent->BodyInstance.bLockTranslation;

	// ��ݷ� �ݿ� ����
	UnitMeshComponent->bApplyImpulseOnDamage = CustomStaticMeshComponent->bApplyImpulseOnDamage;

	// ����
	UnitMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z�� ������ ����
	UnitMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

	// ���忡 ������Ʈ�� ����Ͽ� ���Ϳ� �Բ� �����ǵ��� ���� 
	UnitMeshComponent->RegisterComponent();
	
	// ���� ī��Ʈ
	SpawnCount++;

	// ���� ī��Ʈ ���� üũ �� Ÿ�̸� ����
	if (SpawnCount >= SpawnAmount)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

// ���� �ɼ� ������ �ڵ�� �ϴ� �Լ��ε�, �������⸸ ����. �ʿ� ������ ���߿� ����.
UStaticMeshComponent* ABarBaseActor::InitializeCustomStaticMeshPhysics(UStaticMeshComponent* TargetStaticMesh)
{
	UBodySetup* BodySetup = TargetStaticMesh->GetStaticMesh()->GetBodySetup();
	if (BodySetup)
	{
		bool isCollisionSetup = BodySetup->AggGeom.GetElementCount() > 0;
		if (isCollisionSetup&& !(TargetStaticMesh->IsSimulatingPhysics()))
		{
			UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Enable StaticMesh Physics"));
			TargetStaticMesh->SetSimulatePhysics(true);
			TargetStaticMesh->SetEnableGravity(true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BarBaseActor : Specify StaticMesh First"));
	}

	return TargetStaticMesh;
}

FVector ABarBaseActor::GetStaticMeshBoxUnitSize(UStaticMesh* TargetStaticMesh) const
{
	FVector BoundsExtent = TargetStaticMesh->GetBounds().BoxExtent;
	return BoundsExtent * 2.0f;
}

// ������ �� Procedural Mesh �Ǵ� Ŀ���� �޽� ���� ���� bool�� �����س��� �б��Ͽ� �޽� ���� �Լ� ����
void ABarBaseActor::CreateMesh(float BarHeight)
{
	if (isProceduralMeshUsing)
	{
		CreateProceduralBoxMesh(BarHeight);
	}
	else
	{
		if (CustomStaticMeshComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("BarBaseActor : CreateMesh : Create Custom Static Mesh"));
			CreateCustomMeshRoutine(BarHeight);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BarBaseActor : CreateMesh : Specify Custom Static Mesh First"));
		}
	}
}

// �� �ؽ�Ʈ ������ ����
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{
	// �ؽ�Ʈ ����
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));
}

// �� �ؽ�Ʈ ������ ����
void ABarBaseActor::InitializeTextMeshValue(const float& FloatValue, const float& BarHeight)
{
	int padding = 10;
	
	// �ؽ�Ʈ ����
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));

	// ��ġ
	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TextRenderComponentValue->WorldSize)/2 
		+ padding));
}

// �ִϸ��̼� ���� ����
void ABarBaseActor::PlayBarAnimation()
{
	if (!isProceduralMeshUsing)
	{
		UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Creating Custom Mesh doesn't have animation"));
		return;
	}

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



