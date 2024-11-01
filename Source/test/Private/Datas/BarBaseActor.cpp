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

	// 템플릿으로 둔 커스텀 스태틱 메시의 콜리전 제거
	CustomStaticMeshComponent->SetSimulatePhysics(false);
	CustomStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABarBaseActor::CreateProceduralBoxMesh(float BarHeight)
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

	// Procedural Mesh Component에 메시 생성
	ProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, {}, Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

// 전체 커스텀 스태틱 메쉬 생성 루틴
void ABarBaseActor::CreateCustomMeshRoutine(float BarHeight)
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshComponent->GetStaticMesh()).Z * 
		CustomStaticMeshComponent->GetRelativeScale3D().Z;

	// 생성해야하는 메시 개수 : 
	int32 UnitMeshAmount = BarHeight / UnitMeshHeight;
	UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Amount : %d, UnitSize : %f"), UnitMeshAmount, UnitMeshHeight);

	// 타이머 실행, 0.5초 간격 하드코딩, 람다로 매개변수 있는 함수 캡쳐
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
	{
		CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount); 
	}, CustomMeshSpawnWaitingTime, true);
}

// 박스형태 메시 유닛 사이즈 리턴
FVector ABarBaseActor::GetStaticMeshBoxUnitSize(UStaticMesh* TargetStaticMesh) const
{
	FVector BoundsExtent = TargetStaticMesh->GetBounds().BoxExtent;
	return BoundsExtent * 2.0f;
}

// 커스텀 스태틱 메쉬 생성 함수
void ABarBaseActor::CreateSingleCustomMeshComponent(float BarHeight, float UnitMeshHeight, int32 SpawnAmount)
{
	// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* UnitMeshComponent = NewObject<UStaticMeshComponent>(this);

	InitializeCustomStaticMeshPhysics(UnitMeshComponent, CustomStaticMeshComponent);

	// 부착
	UnitMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z축 스폰 오프셋 조정
	UnitMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

	// 월드에 컴포넌트를 등록하여 액터와 함께 관리되도록 설정 
	UnitMeshComponent->RegisterComponent();
	
	// 스폰 카운트
	SpawnCount++;

	// 스폰 카운트 제한 체크, 
	if (SpawnCount >= SpawnAmount)
	{
		// 타이머 켜져 있는지 체크
		if (GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
		{
			// 타이머 종료
			GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

			// 나머지 BarHeight
			float RestHeight = BarHeight - SpawnAmount*UnitMeshHeight;

			// 나머지 추가적인 메시 생성
			CreateAdditionalCustomMeshComponent(BarHeight, RestHeight, UnitMeshHeight);
		}
	}
}

// 단위로 나누고 나머지 남은 높이, 스케일링 된 유닛 상자 만들어 스폰
void ABarBaseActor::CreateAdditionalCustomMeshComponent(float BarHeight, float RestHeight, float UnitMeshHeight)
{	
	// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* RestMeshComponent = NewObject<UStaticMeshComponent>(this);

	// 물리 형질 복사
	InitializeCustomStaticMeshPhysics(RestMeshComponent, CustomStaticMeshComponent);

	// 스케일 조정
	float scaler = RestHeight / UnitMeshHeight;
	RestMeshComponent->SetRelativeScale3D(RestMeshComponent->GetRelativeScale3D() * FVector(1, 1, scaler));

	// 부착
	RestMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z축 스폰 오프셋 조정
	RestMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

	// 월드에 컴포넌트를 등록하여 액터와 함께 관리되도록 설정 
	RestMeshComponent->RegisterComponent();
}

void ABarBaseActor::InitializeCustomStaticMeshPhysics(UStaticMeshComponent* TargetStaticMesh, UStaticMeshComponent* TemplateComponent)
{
	// 템플릿의 속성을 UnitMeshComponent에 복사
	TargetStaticMesh->SetStaticMesh(TemplateComponent->GetStaticMesh());
	TargetStaticMesh->SetMaterial(0, TemplateComponent->GetMaterial(0));
	TargetStaticMesh->SetRelativeScale3D(TemplateComponent->GetRelativeScale3D());

	// 피직스 복사 : BodyInstance를 직접 복사할 수 있지만, 그 경우에는 AttachToComponent를 사용할 수 없음. -> 개별 복사 필요
	TargetStaticMesh->SetSimulatePhysics(true); // 피직스 on
	TargetStaticMesh->SetEnableGravity(true); // 중력 on
	TargetStaticMesh->SetLinearDamping(TemplateComponent->GetLinearDamping()); // 선형 댐핑
	TargetStaticMesh->SetAngularDamping(TemplateComponent->GetAngularDamping()); // 회전각 댐핑
	//UnitMeshComponent->SetPhysMaterialOverride(CustomStaticMeshComponent->GetBodySetup()->GetPhysMaterial()); // 피지컬 머티리얼
	// 피지컬 머티리얼은 자원 많이 쓰고, 복사하는데도 많은 자원 필요하니, 안함
	TargetStaticMesh->BodyInstance.SetMassOverride(TemplateComponent->BodyInstance.GetMassOverride()); //질량

	// 콜리전 복사 : 콜리전-피직스는 bodyinstance로 묶어 관리 됨. bodyinstance 사용 못하는 현재, 콜리전도 일일이 복사
	TargetStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetStaticMesh->SetCollisionObjectType(TemplateComponent->GetCollisionObjectType());
	TargetStaticMesh->SetCollisionResponseToChannels(TemplateComponent->GetCollisionResponseToChannels());

	// 컨스트레인트 복사
	TargetStaticMesh->BodyInstance.bLockRotation = TemplateComponent->BodyInstance.bLockRotation;
	TargetStaticMesh->BodyInstance.bLockTranslation = TemplateComponent->BodyInstance.bLockTranslation;

	// 충격량 반영 여부
	TargetStaticMesh->bApplyImpulseOnDamage = TemplateComponent->bApplyImpulseOnDamage;
}

// 에디터 상에 Procedural Mesh 또는 커스텀 메시 생성 유무 bool로 추출해놓음 분기하여 메시 생성 함수 결정
void ABarBaseActor::CreateMesh(float BarHeight)
{
	if (!EnableSpawnCustomMesh)
	{
		CreateProceduralBoxMesh(BarHeight);
		UE_LOG(LogTemp, Log, TEXT("Work Done"));
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

// 라벨 텍스트 렌더러 설정
void ABarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{
	// 텍스트 내용
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));
}

// 값 텍스트 렌더러 설정
void ABarBaseActor::InitializeTextMeshValue(const float& FloatValue, const float& BarHeight)
{
	int padding = 10;
	
	// 텍스트 내용
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));

	// 위치
	TextRenderComponentValue->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TextRenderComponentValue->WorldSize)/2 
		+ padding));
}

// 애니메이션 실행 제어
void ABarBaseActor::PlayBarAnimation()
{
	if (EnableSpawnCustomMesh)
	{
		UE_LOG(LogTemp, Log, TEXT("BarBaseActor : Creating Custom Mesh doesn't have animation"));
		return;
	}

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



