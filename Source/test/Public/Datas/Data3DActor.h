// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataManageGameInstance.h"
#include "Data3DActor.generated.h"

class USplineComponent;
class ABarBaseActor;


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

	// ABarBaseActor BP¼­ ÂüÁ¶
	UPROPERTY(EditAnywhere, Category = "BarChart")
	TSubclassOf<ABarBaseActor> BarBase;

	virtual void OnConstruction(const FTransform& Transform) override;


	UFUNCTION(BlueprintCallable, Category = "Data")
	void GetDataAndCreateChart();

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void GenerateShapeChart(const FShapeChartData& CopiedData);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void ClearChildrenActors();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FShapeChartData TestData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarChart")
	float Width_bar = 100.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	// Referenced DataManager
	UPROPERTY()
	UDataManager* DataManagerPtr;
	UFUNCTION(BlueprintCallable)
	void InitilizeDataManager();

};
