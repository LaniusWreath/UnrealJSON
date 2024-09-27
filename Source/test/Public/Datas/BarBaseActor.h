// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarBaseActor.generated.h"


class UTextRenderComponent;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float Width_bar = 10.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// 타임라인에서 불러올 함수
	UFUNCTION()
	void OnAnimationUpdate(float Value);

	// 애니메이션 실행 관리 함수
	UFUNCTION()
	void PlayBarAnimation();

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateBarMesh(float BarHeight);

	UFUNCTION(BlueprintCallable, Category = " Chart")
	void CreateTextMesh(const FString& LabelName, const float& BarHeight, FColor& Color);

//#if WITH_EDITOR
//	// 에디터에서 프로퍼티가 변경될 때 호출되는 함수, AActor에서 상속
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
};
