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
	//SendHTTPGet();
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
	//Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	// Let request know what response we are expecting
	//Request->SetHeader("Content-Type", "application/json");
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
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);	// JsonObject는 포인터임. 신경써야 

			JsonObject = ParseRequestBody(JsonObject);

			//Parsing Json and Put into Variable
			//UKismetMathLibrary::DateTimeFromIsoString(*JsonObject->GetStringField("dateTime"), Time);

			//UE_LOG(LogTemp, Display, TEXT("Current Time: %s"), *Time.ToString());
			//BreakTime();
		}
	}
}

// Rewrite HTTP Request URL with ENUM
void AAPITutorial::SwitchOnCity()
{
	// Base API URL
	CityURL = FString("https://api.odcloud.kr/api/15039554/v1/uddi:b59d0e1a-16c0-44b0-81ea-8c3be4e8a95f?page=1&perPage=10000&serviceKey=bipTqc1mUjvlXa8S4YRREiN7vfjf%2BD33%2BBmtizr1Cz20EakBfihLo%2FSeAw2RdHUk3JLPCvUH2NenPVVtXGhbtQ%3D%3D");

	// Append URL by Construct Member Variable
	/*switch (City)
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
	}*/
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

TSharedPtr<FJsonObject> AAPITutorial::ParseRequestBody(TSharedPtr<FJsonObject>& RequestBody)
{
	const TArray<TSharedPtr<FJsonValue>>* DataArray;
	const TArray<TSharedPtr<FJsonObject>> ResultContainer;

	// data [] 값 가져오기
	if (!RequestBody->TryGetArrayField(TEXT("data"), DataArray))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get 'data' array"));
		return nullptr;
	}

	// 최종 출력 JSON 객체 생성
	TSharedPtr<FJsonObject> OutputJsonObject = MakeShareable(new FJsonObject);

	for (int i = 0; i < DataArray->Num(); i++)
	{
		const TSharedPtr<FJsonObject> CurrentDataObject = (*DataArray)[i]->AsObject();

		// 키와 값을 저장할 배열 생성
		TArray<FString> Regions;
		TArray<float> Values;
		FString ChartTitle;
		FString ChartType = "bar";

		// 키-값 쌍 순회하기
		for (const auto& Pair : CurrentDataObject->Values)
		{
			FString Key = Pair.Key;
			
			// "기준일"은 제외
			if (Key.Equals(TEXT("기준일")))
			{
				ChartTitle = Pair.Value->AsString();
				continue;
			}

			// 키 추가
			Regions.Add(Key);

			// 값 추가
			float Value = Pair.Value->AsNumber();
			Values.Add(Value);
		}

		// 차트 타입, 타이틀 초기화
		OutputJsonObject->SetStringField(TEXT("chartTitle"), ChartTitle);
		OutputJsonObject->SetStringField(TEXT("chartType"), ChartType);

		// "xAxis" 객체 생성
		TSharedPtr<FJsonObject> XAxisObject = MakeShareable(new FJsonObject);
		XAxisObject->SetStringField(TEXT("key"), TEXT("region"));

		// 지역 데이터를 JSON 배열로 변환
		TArray<TSharedPtr<FJsonValue>> RegionJsonArray;
		for (const FString& Region : Regions)
		{
			RegionJsonArray.Add(MakeShareable(new FJsonValueString(Region)));
		}
		XAxisObject->SetArrayField(TEXT("data"), RegionJsonArray);

		// "yAxis" 객체 생성
		TSharedPtr<FJsonObject> YAxisObject = MakeShareable(new FJsonObject);
		YAxisObject->SetStringField(TEXT("key"), TEXT("amount"));
		YAxisObject->SetStringField(TEXT("unit"), TEXT("car"));

		// 값 데이터를 JSON 배열로 변환
		TArray<TSharedPtr<FJsonValue>> ValueJsonArray;
		for (float Value : Values)
		{
			ValueJsonArray.Add(MakeShareable(new FJsonValueNumber(Value)));
		}
		YAxisObject->SetArrayField(TEXT("data"), ValueJsonArray);


		// 최종 JSON 객체에 "xAxis" 및 "yAxis" 추가
		OutputJsonObject->SetObjectField(TEXT("xAxis"), XAxisObject);
		OutputJsonObject->SetObjectField(TEXT("yAxis"), YAxisObject);

		FString OutputJsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJsonString);
		FJsonSerializer::Serialize(OutputJsonObject.ToSharedRef(), Writer);

		// 결과 출력
		UE_LOG(LogTemp, Log, TEXT("Output JSON: %s"), *OutputJsonString);
	}
	ParsedJsonObjectArray.Add(OutputJsonObject);
	return OutputJsonObject;
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
