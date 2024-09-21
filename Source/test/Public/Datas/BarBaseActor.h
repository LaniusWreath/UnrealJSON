// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarBaseActor.generated.h"

class UTimelineComponent;
class UProceduralMeshComponent;

UCLASS()
class TEST_API ABarBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarBaseActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	UProceduralMeshComponent* ProcMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* MeshMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	UTimelineComponent* BarAnimationTimeline;

	UPROPERTY(EditAnywhere, Category = "Chart")
	UCurveFloat* AnimationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float Width_bar = 10.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Ÿ�Ӷ��ο��� �ҷ��� �Լ�
	UFUNCTION()
	void OnAnimationUpdate(float Value);

	// �ִϸ��̼� ���� ���� �Լ�
	UFUNCTION()
	void PlayBarAnimation();

	UFUNCTION(BlueprintCallable, Category = "Cahrt")
	void CreateBarMesh(float BarHeight);

//#if WITH_EDITOR
//	// �����Ϳ��� ������Ƽ�� ����� �� ȣ��Ǵ� �Լ�, AActor���� ���
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
};
