// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JCMDataTypes.h"
#include "JCM3DChartActor.generated.h"

class UJCMDataManager;
class UJCMDataContainer;
class UJCMDataContainerBar;
class UChartGenerator;
class UTextRenderComponent;
class UHTTPRequestHandler;

UCLASS()
class TEST_API AJCM3DChartActor : public AActor
{
	GENERATED_BODY()

public:	
	AJCM3DChartActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetJsonObject(const TSharedPtr<FJsonObject> JsonData);

	void SetJsonString(const bool IsWorkDone);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	FString CurrentChartType;
	
	// Call Json Request Function, Result Data will be stored in this  
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void RequestJsonObject(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void RequestJsonString(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	const UJCMHttpHandler* GetHttpRequestHandler() const
	{
		if (RequestManagerInstance){
			return RequestManagerInstance;
		}
		else
			return nullptr;
	}

	// Visualization Chart Title
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartTitle;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Chart")
	bool IsDataClassInstanceSet;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Initializing Data Manager Getting from Game Instance
	UFUNCTION()
	void InitializeDataManager();

	// Initializing Request Manager Instance : this instance have to be initialized in every CallJsonRoutine()
	UFUNCTION()
	void InitializeRequestManager();

	// Pure Virtual Routine for Generate Chart
	virtual void GenerateChartRoutine() PURE_VIRTUAL(UDataFetcherBase::FetchData, ;);

	// DataManager Reference
	UPROPERTY()
	UJCMDataManager* DataManagerInstance;

	// Data Class Instance
	UPROPERTY(VisibleAnywhere, Category = "Chart")
	UJCMDataContainer* DataClassInstance;

	UPROPERTY()
	UJCMHttpHandler* RequestManagerInstance;

	UPROPERTY()
	USceneComponent* RootSceneComponent;

	bool IsRequestJsonStringDone;

};

class UJCMChartGeneratorBar;
class AJCMBarBaseActor;

UCLASS(Blueprintable)
class AJCM3DChartActorBar : public AJCM3DChartActor
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void SetChartDefaultTexts();

protected:
	//virtual UDataClasses* SetDataClassInstance() override;
	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void GenerateChartRoutine() override;

public:
	AJCM3DChartActorBar();

	// On : Spawn Chart Mesh At Each Spline Point / Off : Spawn Chart Mesh by Equally Deviding whole Spline
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chart")
	bool EnableGenerateMeshAtSplinePoint;

	// Select Bar Blueprint Actor Source to Generate
	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "Chart")
	TSubclassOf<AJCMBarBaseActor> BarBaseActorBPClass;

	// Controler Component for Generating 3D Bar Chart 
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	UJCMChartGeneratorBar* BarGeneratorComponent;

	// Visualization Chart Xaxis Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartXaxisName;

	// Visualization Chart Yaxis Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	UTextRenderComponent* TextRenderComponent_chartYaxisName;

	// json�� �޾ƿ� DataClass��� ���� ������ �����̳� Ŭ������ �������� �����ϰ�����.
	// ���� �� ���� �����ʹ� ���� ���� �߻�Ŭ������ ��������
	// �������Ʈ������ ���Ͱ� �ڽ� Ŭ������ ��üȭ�Ǿ������Ƿ�, ������ �����̳� Ŭ������ ȣ�� �� ������ ���� ���Ŀ� �°�
	// ĳ�������ִ� ������ �ʿ���. �ٸ� �ڽ� 3DActor Ŭ�������� �������� ��.
	
	//Get Precessed Json Data Container Class
	UFUNCTION(BlueprintCallable, Category = "Chart")
	const UJCMDataContainer* GetDataClassInstance() const
	{
		if (IsDataClassInstanceSet)
			return DataClassInstance;
		else
			return nullptr;
	}
	
	// Set Processed Json Data Container Class Instance Directly : You have to get a reference from other Data3DActor Instance to use this function.
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void SetDataClassInstance(UJCMDataContainer* DataClassInstancePtr)
	{
		DataClassInstance = DataClassInstancePtr;
		IsDataClassInstanceSet = true;
	}
	
	// Delete Data Container Class Instance
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void DeleteClassInstance()
	{
		DataClassInstance = nullptr;
		IsDataClassInstanceSet = false;
	}
};