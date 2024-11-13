// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMBarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/TextRenderComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BodyInstance.h"
#include "TimerManager.h"

// Sets default values
AJCMBarBaseActor::AJCMBarBaseActor()
{
	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRootComponent;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("BasicMesh"));
	ProcMeshComponent->SetupAttachment(RootComponent);

	CustomActorSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CustomMeshOffsetLayer"));
	CustomActorSceneComponent->SetupAttachment(RootComponent);

	CustomStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomMesh"));
	CustomStaticMeshComponent->SetupAttachment(RootComponent);

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
void AJCMBarBaseActor::BeginPlay()
{
	Super::BeginPlay();

	// ���ø����� �� Ŀ���� ����ƽ �޽��� �ݸ��� ����
	CustomStaticMeshComponent->SetSimulatePhysics(false);
	CustomStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AJCMBarBaseActor::CreateProceduralBoxMesh(float BarHeight)
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

// Ŀ���� ����ƽ �޽� ���� ��ƾ : ���� �ڵ� ���
void AJCMBarBaseActor::CreateCustomMeshRoutine(float BarHeight)
{
	// Ŀ���� �޽� ���� ���� : ���� ���� * ���� �����Ϸ�
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshComponent->GetStaticMesh()).Z * 
		CustomStaticMeshComponent->GetRelativeScale3D().Z;

	// �����ؾ��ϴ� �޽� ���� : 
	int32 UnitMeshAmount;
	UnitMeshAmount = BarHeight / UnitMeshHeight;
	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Amount : %d, UnitSize : %f"), UnitMeshAmount, UnitMeshHeight);

	// Ÿ�̸� ����, ���ٷ� �Ű����� �ִ� �Լ� ĸ��
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
	{
		CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount); 
	}, CustomMeshSpawnWaitingTime, true);
}

// ������ ��Ʈ �޽� ���� : ����� ���� ����
void AJCMBarBaseActor::CreateCustomMeshRoutine(float BarHeight, int amount)
{
	// Ŀ���� �޽� ���� ���� : ���� ���� * ���� �����Ϸ�
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshComponent->GetStaticMesh()).Z *
		CustomStaticMeshComponent->GetRelativeScale3D().Z;

	// �����ؾ��ϴ� �޽� ���� : 
	int UnitMeshAmount = amount;
	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Amount : %d, UnitSize : %f"), UnitMeshAmount, UnitMeshHeight);

	// Ÿ�̸� ����, ���ٷ� �Ű����� �ִ� �Լ� ĸ��
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
		{
			CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount);
		}, CustomMeshSpawnWaitingTime, true);
}

// �ڽ����� �޽� ���� ������ ����
FVector AJCMBarBaseActor::GetStaticMeshBoxUnitSize(UStaticMesh* TargetStaticMesh) const
{
	FVector BoundsExtent = TargetStaticMesh->GetBounds().BoxExtent;
	return BoundsExtent * 2.0f;
}

// Ŀ���� ����ƽ �޽� ���� �Լ�
void AJCMBarBaseActor::CreateSingleCustomMeshComponent(float BarHeight, float UnitMeshHeight, int32 SpawnAmount)
{
	// ���� ī��Ʈ ���� üũ, 
	if (SpawnCount >= SpawnAmount)
	{
		ClearSpawnTimerHandle();
	}
	else
	{
		// StaticMeshComponent�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
		UStaticMeshComponent* UnitMeshComponent = NewObject<UStaticMeshComponent>(this);

		InitializeCustomStaticMeshPhysics(UnitMeshComponent, CustomStaticMeshComponent);

		// ����
		UnitMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

		// Z�� ���� ������ ����
		if (SpawnPerUnitValue)
		{
			UnitMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight * SpawnAmount + 1));
		}
		else {
			UnitMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

		}

		// ���忡 ������Ʈ�� ����Ͽ� ���Ϳ� �Բ� �����ǵ��� ���� 
		UnitMeshComponent->RegisterComponent();

		// ���� ī��Ʈ
		SpawnCount++;
	}
}

// ���� ������ ����ƽ �޽� ������Ʈ ����
void AJCMBarBaseActor::ClearCustomMeshes()
{
	if (CustomActorSceneComponent->GetNumChildrenComponents() > 0)
	{
		TArray<USceneComponent*> AttachedComponents;
		CustomActorSceneComponent->GetChildrenComponents(false, AttachedComponents);

		for (USceneComponent* ChildComponent : AttachedComponents)
		{
			if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(ChildComponent))
			{
				MeshComponent->DestroyComponent();
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("All CustomStaticMesh Components cleard"));
	}
}

// �޽� ���� Ÿ�̸� �ڵ� �ʱ�ȭ : BarBaseActor�� BarGenerator���� Gernating �� �� ���� ���ο� �ν��Ͻ��� �����Ǿ� �����.
// ���� ���� BarBaseActor�� �޽� ���� Ÿ�̸Ӹ� ������Ű�� ���� ���� �ν��Ͻ��� �����ϰ� �ִ� BarGenerator���� ���� ������� ��.
void AJCMBarBaseActor::ClearSpawnTimerHandle()
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		// Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		// Ÿ�̸� ����� ȣ���� �Լ��� ���⿡ �ۼ�
	}
}

// ������ ������ ������ ���� ����, �����ϸ� �� ���� ���� ����� ���� : ����� �Ⱦ�
void AJCMBarBaseActor::CreateAdditionalCustomMeshComponent(float BarHeight, float RestHeight, float UnitMeshHeight)
{	
	// StaticMeshComponent�� �������� �����ϰ�, �θ� ���Ϳ� ���ϵ��� ����
	UStaticMeshComponent* RestMeshComponent = NewObject<UStaticMeshComponent>(this);

	// ���� ���� ����
	InitializeCustomStaticMeshPhysics(RestMeshComponent, CustomStaticMeshComponent);

	// ������ ����
	float scaler = RestHeight / UnitMeshHeight;
	RestMeshComponent->SetRelativeScale3D(RestMeshComponent->GetRelativeScale3D() * FVector(1, 1, scaler));

	// ����
	RestMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z�� ���� ������ ����
	RestMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

	// ���忡 ������Ʈ�� ����Ͽ� ���Ϳ� �Բ� �����ǵ��� ���� 
	RestMeshComponent->RegisterComponent();
}

// ���ø� �޽��κ��� �Ӽ� ����
void AJCMBarBaseActor::InitializeCustomStaticMeshPhysics(UStaticMeshComponent* TargetStaticMesh, UStaticMeshComponent* TemplateComponent)
{
	// ���ø��� �Ӽ��� UnitMeshComponent�� ����
	TargetStaticMesh->SetStaticMesh(TemplateComponent->GetStaticMesh());
	TargetStaticMesh->SetMaterial(0, TemplateComponent->GetMaterial(0));
	TargetStaticMesh->SetRelativeScale3D(TemplateComponent->GetRelativeScale3D());

	// ������ ���� : BodyInstance�� ���� ������ �� ������, �� ��쿡�� AttachToComponent�� ����� �� ����. -> ���� ���� �ʿ�
	TargetStaticMesh->SetSimulatePhysics(true); // ������ ���� ����
	TargetStaticMesh->SetEnableGravity(TemplateComponent->IsGravityEnabled()); // �߷� on
	TargetStaticMesh->SetLinearDamping(TemplateComponent->GetLinearDamping()); // ���� ����
	TargetStaticMesh->SetAngularDamping(TemplateComponent->GetAngularDamping()); // ȸ���� ����
	//UnitMeshComponent->SetPhysMaterialOverride(CustomStaticMeshComponent->GetBodySetup()->GetPhysMaterial()); // ������ ��Ƽ����
	// ������ ��Ƽ������ �ڿ� ���� ����, �����ϴµ��� ���� �ڿ� �ʿ��ϴ�, ����
	TargetStaticMesh->BodyInstance.SetMassOverride(TemplateComponent->BodyInstance.GetMassOverride()); //����

	// �ݸ��� ���� : �ݸ���-�������� bodyinstance�� ���� ���� ��. bodyinstance ��� ���ϴ� ����, �ݸ����� ������ ����
	TargetStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetStaticMesh->SetCollisionObjectType(TemplateComponent->GetCollisionObjectType());
	TargetStaticMesh->SetCollisionResponseToChannels(TemplateComponent->GetCollisionResponseToChannels());

	// ����Ʈ����Ʈ ����
	TargetStaticMesh->BodyInstance.bLockRotation = TemplateComponent->BodyInstance.bLockRotation;
	TargetStaticMesh->BodyInstance.bLockTranslation = TemplateComponent->BodyInstance.bLockTranslation;

	// ��ݷ� �ݿ� ����
	TargetStaticMesh->bApplyImpulseOnDamage = TemplateComponent->bApplyImpulseOnDamage;
}

// ������ �� Procedural Mesh �Ǵ� Ŀ���� �޽� ���� ���� bool�� �����س��� �б��Ͽ� �޽� ���� �Լ� ����
void AJCMBarBaseActor::CreateMesh(float BarHeight, int Value)
{
	// ���ν����� �޽�
	if (!EnableSpawnCustomMesh)
	{
		CreateProceduralBoxMesh(BarHeight);	// �޽� ����
		AdjustTextMeshValueOffset(BarHeight); // �ؽ�Ʈ ���� ������Ʈ ������ ����
	}
	// Ŀ���� �޽�
	else
	{
		if (CustomStaticMeshComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("BarBaseActor : CreateMesh : Create Custom Static Mesh"));
			// ���� �����Ͽ� �޽� ����
			if (SpawnPerUnitValue)
			{
				// ����� ���� ������ ���� 
				int Amount = Value / UnitValue;
				CreateCustomMeshRoutine(BarHeight, Amount);
				AdjustTextMeshValueOffset(Amount);
			}
			// ���� �ڵ� ����Ͽ� �޽� ����
			else
			{
				CreateCustomMeshRoutine(BarHeight);
				AdjustTextMeshValueOffset(BarHeight);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BarBaseActor : CreateMesh : Specify Custom Static Mesh First"));
		}
	}
}

// �� �ؽ�Ʈ ������ ����
void AJCMBarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{
	// �ؽ�Ʈ ����
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));
}

// �� �ؽ�Ʈ ������ ����
void AJCMBarBaseActor::InitializeTextMeshValue(const float& FloatValue)
{
	// �ؽ�Ʈ ����
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));
}

// ��Ȯ�� ���̿� �ؽ�Ʈ ���� ������Ʈ ��ġ
void AJCMBarBaseActor::AdjustTextMeshValueOffset(const float& BarHeight)
{
	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TextRenderComponentValue->WorldSize) / 2
			+ TextRenderComponentOffset_Value));
}

// Ŀ���Ҹ޽� ������ ������ ���� ���̿� �ؽ�Ʈ ���� ������Ʈ ��ġ
void AJCMBarBaseActor::AdjustTextMeshValueOffset(const int& amount)
{
	// Ŀ���� �޽� ���� ���� : ���� ���� * ���� �����Ϸ�
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshComponent->GetStaticMesh()).Z *
			CustomStaticMeshComponent->GetRelativeScale3D().Z;

	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, (UnitMeshHeight*amount + (TextRenderComponentValue->WorldSize / 2)
		+ TextRenderComponentOffset_Value)));
}

void AJCMBarBaseActor::BindTimelineAnimation()
{
	// �ִϸ��̼� �ʱ�ȭ
	if (AnimationCurve && !EnableSpawnCustomMesh)
	{
		FOnTimelineFloat TimelineCallBack;
		// Ÿ�Ӷ��ο� �Լ� ���ε�
		TimelineCallBack.BindUFunction(this, FName("OnAnimationUpdate"));

		// Timeline�� Curve�� Curve�� ����� Callback �Լ� �߰�
		BarAnimationTimeline->AddInterpFloat(AnimationCurve, TimelineCallBack);
		BarAnimationTimeline->SetLooping(false);
		BarAnimationTimeline->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BarBaseActor : Failed finding Animation Cuvrve"));
	}
}

// �ִϸ��̼� ���� ����
void AJCMBarBaseActor::PlayBarAnimation()
{
	if (EnableSpawnCustomMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("BarBaseActor : Creating Custom Mesh doesn't have animation"));
		return;
	}
	BarAnimationTimeline->PlayFromStart();
}

// ���� ������ ���� �ִϸ��̼� ����
void AJCMBarBaseActor::OnAnimationUpdate(float Value)
{
	FVector CurrentScale = GetActorScale();
	SetActorScale3D(FVector(CurrentScale.X, CurrentScale.Y, Value));
}



