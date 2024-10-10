// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTypes.h"
#include "Data3DActor.generated.h"

class UDataManager;
class UDataClasses;
class UChartGenerator;

UCLASS(Abstract)
class TEST_API AData3DActor : public AActor
{
	GENERATED_BODY()

public:	
	AData3DActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CurrentChartType;

	// Chart Generator Component Class Generating what Chart Type. This Member Decides How the Chart Type to Be. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartSettings", meta = (DisplayPriority = 0))
	TSubclassOf<UChartGenerator> ChartGeneratorComponentClass;

	// Initializing Data Manager Getting from Game Instance
	UFUNCTION()
	void InitilizeDataManager();


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
	// Controler Component for Generating 3D Bar Chart 
	UPROPERTY()
	UBarGenerator* BarGeneratorComponent;

protected:
	virtual void SetDataClassInstance() override;
	virtual void GenerateChartRoutine() override;

public:
	AData3DActorBar();

	// Select Bar Blueprint Actor Source to Generate
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "Chart")
	TSubclassOf<ABarBaseActor> BarBaseActorBPClass;

};