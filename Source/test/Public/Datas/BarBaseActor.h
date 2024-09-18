// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralmeshComponent.h"
#include "BarBaseActor.generated.h"


UCLASS()
class TEST_API ABarBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarBaseActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Mesh")
	UProceduralMeshComponent* ProcMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Mesh")
	UStaticMeshComponent* BarBaseMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarChart")
	float Width_bar = 100.f;


	void CreateBarMesh(float BarHeight);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
