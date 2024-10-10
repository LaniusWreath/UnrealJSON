// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h" 
#include "DataTypes.h"
#include "ChartGenerator.generated.h"
/**
 * 
 */

// ChartGenerator Abstract Component Class
UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST_API UChartGenerator : public USceneComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RootMeshComponent;

	// This Array contains Chart Base BP Actor 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TArray<UChildActorComponent*> ChildActorComponents;

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();

	virtual void BeginPlay() override;

public:
	UChartGenerator();

};

class UArrowComponent;
class USplineComponent;
class ABarBaseActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UBarGenerator : public UChartGenerator
{
	GENERATED_BODY()

private:
	// For Visually Modulation Bar Chart with Deviagtion 
	float DeviationScaler = 1;

	// Chart Value PreProcess function
	UFUNCTION()
	void PrepareBarValues(const TArray<float>& ValueArray, float& AverageHeightResult, float& BarHeightScalerResult,
		const int SplineLength, const int MaxHeight);

	// Create BarChart Function
	UFUNCTION()
	bool CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing,
		const float AverageHeight, const float BarHeightScaler);

	// BP BarBase Actor Source to Display 
	UPROPERTY(VisibleAnywhere, Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

public:
	UBarGenerator();

	// Generate Bar Chart Routine Function
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GenerateBarChart(const FShapeChartData& CopiedData);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UArrowComponent* ArrowComponent;

	// Set BP BarBase Actor Source to Display 
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

