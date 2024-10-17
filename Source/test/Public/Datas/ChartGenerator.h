// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h" 
#include "DataTypes.h"
#include "ChartGenerator.generated.h"
/**
 * 
 */

class UTextRenderComponent;

// ChartGenerator Abstract Component Class
UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST_API UChartGenerator : public USceneComponent
{
	GENERATED_BODY()

protected:

	// This Array contains Chart Base BP Actor 
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TArray<UChildActorComponent*> ChildActorComponents;

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();

	virtual void BeginPlay() override;

public:
	UChartGenerator();

	UPROPERTY(BlueprintReadOnly)
	USceneComponent* ChildActorContainComponent;

};

class USplineComponent;
class ABarBaseActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UBarGenerator : public UChartGenerator
{
	GENERATED_BODY()

private:
	// For Visually Modulation Bar Chart with Deviagtion 
	float CustomScaleValue = 1;
	float CustomPaddingScaleValue = 1;

	// Chart Value PreProcess function
	UFUNCTION()
	void PrepareBarValues(const TArray<float>& ValueArray, float& BarHeightScalerResult, float& BarPaddingResult,
		const float MaxHeight);

	// Create BarChart Function
	UFUNCTION()
	bool CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing,
		const float BarPaddingScaler, const float BarHeightScaler);

	// BP BarBase Actor Source to Display 
	UPROPERTY()
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

public:
	UBarGenerator();

	// Generate Bar Chart Routine Function
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GenerateBarChart(const FShapeChartData& CopiedData);

	// Set BP BarBase Actor Source to Display 
	UFUNCTION()
	void SetBarSourceActor(const TSubclassOf<ABarBaseActor>& SourceActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	USplineComponent* SplineComponent_length;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	USplineComponent* SplineComponent_height;
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

