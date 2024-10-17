// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "APITutorial.generated.h"

/**
 * 
 */

// Let Blueprint read Enum
UENUM(BlueprintType)
enum class ECity :uint8
{
	// UMETA : Describe How Blueprint to show this value
	EC_Tokyo	UMETA(DisplayName = "Tokyo"),
	EC_London	UMETA(DisplayName = "London"),
	EC_NewYork	UMETA(DisplayName = "New York"),
	EC_Default	UMETA(DisplayName = "Default"),
};

UCLASS()
class TEST_API AAPITutorial : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Counstructor, should be changed
	AAPITutorial();

protected:
	
	virtual void BeginPlay() override;

	// Used to send HTTP Reuqest
	UFUNCTION(BlueprintCallable, Category = "Request")
	void SendHTTPGet();

	// Handle the HttpRequest Response
	void OnGetTimeResponse(FHttpRequestPtr Request, FHttpResponsePtr Reponse, bool bConnectedSuccesfully);

	// Rewrite HTTP Request URL with ENUM
	void SwitchOnCity();

	void BreakTime();

	TSharedPtr<FJsonObject> ParseRequestBody(TSharedPtr<FJsonObject>& RequestBody);

private:

	// Manage HTTP Request Module
	FHttpModule* Http;

	// Request Parameters
	ECity City;
	FString CityURL;

	TSharedPtr<FJsonObject> NullJsonObjectPtr;

	// Request Result Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Time, meta = (AllowPrivateAccess = "true"))
	FDateTime Time;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Response, meta = (AllowPrivateAccess = "true"))
	FString ResponseString;

	int32 Hour;
	int32 Minute;
	int32 Second;

	TArray<TSharedPtr<FJsonObject>> ParsedJsonObjectArray;

public:
	UFUNCTION(BlueprintCallable)
	FText GetCurrentTime();

	UFUNCTION(BlueprintCallable)
	FText GetResponseBody();

	const TSharedPtr<FJsonObject>& GetParsedJsonObject(int index);
};
