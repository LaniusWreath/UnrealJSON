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

	UPROPERTY()
	USceneComponent* DefaultSceneRootComponent;

	UPROPERTY()
	UProceduralMeshComponent* ProcMeshComponent;

	// TimeLine Animation Binding Function
	UFUNCTION()
	void OnAnimationUpdate(float Value);

	UFUNCTION()
	void CreateLegacyMesh(float BarHeight);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ABarBaseActor();

	UPROPERTY(EditAnywhere, Category = "Chart")
	bool isProceduralMeshUsing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chart")
	USceneComponent* LegacyActorSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chart")
	UStaticMeshComponent* LegacyStaticMeshComponent;

	// Bar Procedural Mesh Material
	UPROPERTY(EditAnywhere, Category = "Chart")
	UMaterialInstance* MeshMaterial;

	// Bar Generate Animation Curve
	UPROPERTY(EditAnywhere, Category = "Chart")
	UCurveFloat* AnimationCurve;

	UPROPERTY()
	UTextRenderComponent* TextRenderComponentValue;

	UPROPERTY()
	UTextRenderComponent* TextRenderComponentLabel;

	// Text Mesh Color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FColor TextColor = FColor::Blue;

	// Bar Mesh Width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float Width_bar = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	int UnitSize = 1;

	// Bar Text Unit Size : Label
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float ValueTextPadding = 15.f;

	// Animation Control Function
	UFUNCTION()
	void PlayBarAnimation();

	// Create Procedural Mesh
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateProceduralBoxMesh(float BarHeight);

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void CreateMesh(float BarHeight);

	// Initialize Lbel Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshLabel(const FString& LabelName);

	// Initialize Value Text Mesh
	UFUNCTION(BlueprintCallable, Category = " Chart")
	void InitializeTextMeshValue(const float& FloatValue, const float& BarHeight);


};
