// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h" 
#include "DataTypes.h"
#include "ChartGenerator.generated.h"
/**
 * 
 */
class UArrowComponent;
class USplineComponent;
class ABarBaseActor;

// 컴포넌트 
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST_API UChartGenerator : public USceneComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RootMeshComponent;

	// 차트 요소 액터가 담길 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TArray<UChildActorComponent*> ChildActorComponents;

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();

	virtual void BeginPlay() override;

public:
	UChartGenerator();

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UBarGenerator : public UChartGenerator
{
	GENERATED_BODY()

private:
	// 연출용 편차 계수
	float DeviationScaler = 1;

	UFUNCTION()
	void PrepareBarValues(const TArray<float>& ValueArray, float& AverageHeightResult, float& BarHeightScalerResult,
		const int SplineLength, const int MaxHeight);

	UFUNCTION()
	bool CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing,
		const float AverageHeight, const float BarHeightScaler);

	// ABarBaseActor BP서 참조, Data3DActorBar에서 할당
	UPROPERTY(VisibleAnywhere, Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

public:
	UBarGenerator();

	// 바 생성 루틴 호출 함수
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GenerateBarChart(const FShapeChartData& CopiedData);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UArrowComponent* ArrowComponent;

	UFUNCTION()
	void SetBarSourceActor(const TSubclassOf<ABarBaseActor>& SourceActor);

};

UCLASS(Blueprintable)
class ULineGenerator : public UChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UPieGenerator : public UChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UXYGenerator : public UChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UXYZGenerator : public UChartGenerator
{
	GENERATED_BODY()

public:
};

