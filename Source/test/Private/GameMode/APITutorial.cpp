// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/APITutorial.h"
#include "Kismet/KismetMathLibrary.h"


AAPITutorial::AAPITutorial():
	//Construct with Default Class Member
	City(ECity::EC_Tokyo)
{
	Http = &FHttpModule::Get();
}

void AAPITutorial::BeginPlay()
{
	Super::BeginPlay();
	SendHTTPGet();

}

void AAPITutorial::SendHTTPGet()
{
	SwitchOnCity();

	// Creating Request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// Bind response function to request
	Request->OnProcessRequestComplete().BindUObject(this, &AAPITutorial::OnGetTimeResponse);
	// Setting the URL - Where to send the request
	Request->SetURL(CityURL);
	// Adding what type of request
	Request->SetVerb("GET");
	// Let Server know what applicaation sent the request
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	// Let request know what response we are expecting
	Request->SetHeader("Content-Type", "application/json");
	// Sending the request
	Request->ProcessRequest();
}

void AAPITutorial::OnGetTimeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully)
{
	// Creating the Json Object we will be woring with
	TSharedPtr<FJsonObject> JsonObject;
	
	UE_LOG(LogTemp, Display, TEXT("Response Result : %d"),Response->GetResponseCode());

	// Check if we are successful in our request
	if (Response->GetResponseCode() == 200)
	{
		// Store the response in our response body
		const FString ResponseBody = Response->GetContentAsString();
		// Creating a reader for the response body
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
		// Check if we are successful in reading the json
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// This is where we put information on what to do with the data
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			//Parsing Json and Put into Variable
			UKismetMathLibrary::DateTimeFromIsoString(*JsonObject->GetStringField("dateTime"), Time);

			UE_LOG(LogTemp, Display, TEXT("Current Time: %s"), *Time.ToString());
			BreakTime();
		}
	}
}

// Rewrite HTTP Request URL with ENUM
void AAPITutorial::SwitchOnCity()
{
	// Base API URL
	CityURL = FString("https://timeapi.io/api/time/current/zone?timeZone=");

	// Append URL by Construct Member Variable
	switch (City)
	{
	case ECity::EC_Tokyo:
		CityURL.Append("Asia/Tokyo");
		break;
	case ECity::EC_London:
		CityURL.Append("Europe/London");
		break;
	case ECity::EC_NewYork:
		CityURL.Append("America/NewYork");
		break;
	case ECity::EC_Default:
		break;
	default:
		break;
	}
}

void AAPITutorial::BreakTime()
{
	int32 Year;
	int32 Month;
	int32 Day;
	int32 MiliSecond;

	// Break the Time Variable and store the data within individual Variable
	UKismetMathLibrary::BreakDateTime(Time, Year, Month, Day, Hour, Minute, Second, MiliSecond);
	UE_LOG(LogTemp, Log, TEXT("Time is: %d"), Hour);
	UE_LOG(LogTemp, Log, TEXT("Time is: %d"), Minute);
	UE_LOG(LogTemp, Log, TEXT("Time is: %d"), Second);

}

FText AAPITutorial::GetCurrentTime()
{
	FString Hours;
	FString Minutes;
	FString Seconds;

	if (Hour < 10)
	{
		Hours = FString("0").Append(FString::FromInt(Hour));
	}
	else
	{
		Hours = FString::FromInt(Hour);
	}
	if (Minute < 10)
	{
		Hours = FString("0").Append(FString::FromInt(Minute));
	}
	else
	{
		Hours = FString::FromInt(Minute);
	}
	if (Second < 10)
	{
		Hours = FString("0").Append(FString::FromInt(Second));
	}
	else
	{
		Hours = FString::FromInt(Second);
	}

	FString ReturnString = Hours.Append(FString(":")).Append(Minutes).Append(FString(":")).Append(Seconds);

	return FText::FromString(ReturnString);
}

// Get Full Response Body
FText AAPITutorial::GetResponseBody()
{
	return FText::FromString(ResponseString);
}
