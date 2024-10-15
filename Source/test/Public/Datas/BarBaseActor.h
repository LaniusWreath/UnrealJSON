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

private:

	UPROPERTY()
	UTimelineComponent* BarAnimationTimeline;

	// TimeLine Animation Binding Function
	UFUNCTION()
	void OnAnimationUpdate(float Value);

	UPROPERTY()
	USceneComponent* DefaultSceneRootComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ABarBaseActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chart")
	UProceduralMeshComponent* ProcMeshComponent;

	// Bar Procedural Mesh Material
	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInstance* MeshMaterial;

	// Bar Generate Animation Curve
	UPROPERTY(EditAnywhere, Category = "Chart")
	UCurveFloat* AnimationCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Chart")
	UTextRenderComponent* TextRenderComponentValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Chart")
	UTextRenderComponent* TextRenderComponentLabel;

	// Text Mesh Color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FColor TextColor = FColor::Blue;

	// Bar Mesh Width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float Width_bar = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cahrt")
	int UnitSize = 1;

	// Bar Text Unit Size : Label
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float ValueTextPadding = 15.f;

	// Animation Control Function
	UFUNCTION()
	void PlayBarAnimation();

	// Create Procedural Mesh
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateBarMesh(float BarHeight);

	// Create Procedural Mesh
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateBoxMesh(float BarHeight);

	// Initialize Lbel Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshLabel(const FString& LabelName);

	// Initialize Value Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshValue(const float& FloatValue, const float& BarHeight);
};
