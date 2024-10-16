// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes.h"
#include "Data3DActor.generated.h"

class UDataManager;
class UDataClasses;
class UChartGenerator;
class UTextRenderComponent;

UCLASS(Abstract)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()

public:	
	AData3DActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Chart")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	FString CurrentChartType;

	// Initializing Data Manager Getting from Game Instance
	UFUNCTION()
	void InitilizeDataManager();

	// Visualization Chart Title
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartTitle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Pure Virtual Setter Data Class Instance. Getting from DataManager's ChartDataClassInstanceArray.
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void SetDataClassInstance() PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// Pure Virtual Routine for Generate Chart
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void GenerateChartRoutine()  PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// DataManager Reference
	UPROPERTY()
	UDataManager* DataManagerReference;

	// Data Class Instance
	UPROPERTY()
	UDataClasses* DataClassInstance;

};

class UBarGenerator;
class ABarBaseActor;

UCLASS(Blueprintable)
class AData3DActorBar : public AData3DActor
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void SetChartLabelText();

protected:
	virtual void SetDataClassInstance() override;
	virtual void GenerateChartRoutine() override;

public:
	AData3DActorBar();

	// Controler Component for Generating 3D Bar Chart 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	UBarGenerator* BarGeneratorComponent;

	// Select Bar Blueprint Actor Source to Generate
	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

	// Visualization Chart Xaxis Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartXaxisName;

	// Visualization Chart Yaxis Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartYaxisName;

};