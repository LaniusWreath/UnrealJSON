// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes.h"
#include "Data3DActor.generated.h"

class UDataManageGameInstance;
class UDataManager;
class USplineComponent;
class ABarBaseActor;
class UArrowComponent;

UCLASS(Blueprintable)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()

public:	
	AData3DActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UArrowComponent* ArrowComponent;

	// ABarBaseActor BP서 참조
	UPROPERTY(EditAnywhere, Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TArray<UChildActorComponent*> ChildActorComponents;


	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GetDataAndCreateChart();

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateShapeChart(const FShapeChartData& CopiedData);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	bool PrepareBarValues(const TArray<float>& ValueArray, float& AverageHeightResult, float& BarHeightScalerResult, int SplineLength, int MaxHeight);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	bool GenerateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing, const float AverageHeight, const float BarHeightScaler);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void PlayChildrenAnimation();

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();

	// 편차 강제로 늘리는 값(되도록 0~1까지만)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float DeviationScaler = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FShapeChartData TestShapeData;


	UFUNCTION(CallInEditor, Category = "Chart")
	void UpdateInEditor();

	//virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// DataManager Reference
	UPROPERTY()
	UDataManager* DataManagerPtr;
	UFUNCTION(BlueprintCallable)
	void InitilizeDataManager();

};
