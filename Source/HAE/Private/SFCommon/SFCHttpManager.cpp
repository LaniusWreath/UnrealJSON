// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCHttpManager.h"
#include "SFCommon/SFCLog.h"

// ���� request �Լ�
void USFCHttpManager::MakeGetRequest(const FString& Url, const bool GetResultWithFString)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

	// ���� �Լ� ��������Ʈ ���ε�.
	if (GetResultWithFString)
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceivedWithString);
	}
	else
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceivedWithPtr);
	}

	// ��û ����.
	Request->ProcessRequest();
}

// ��� �����ؼ� ��û
void USFCHttpManager::MakeGetRequestWithHeader(const FString& Url, const TMap<FString, FString>& Headers, 
	const TMap<FString, FString>& Parameters, const bool GetResultWithFString)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// URL�� �Ķ���� �߰�
	FString FinalUrl = Url;
	if (Parameters.Num() > 0)
	{
		FinalUrl += TEXT("?");
		for (const TPair<FString, FString>& Param : Parameters)
		{
			FinalUrl += FString::Printf(TEXT("%s=%s&"), *Param.Key, *Param.Value);
		}
		// ������ '&' ����
		FinalUrl.RemoveFromEnd(TEXT("&"));
	}

	// ��û ����
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(FinalUrl);
	Request->SetVerb(TEXT("GET"));

	// ��� ����
	for (const TPair<FString, FString>& Header : Headers)
	{
		Request->SetHeader(Header.Key, Header.Value);
	}

	// ���� �Լ� ��������Ʈ ���ε�
	if (GetResultWithFString)
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceivedWithString);
	}
	else
	{
		Request->OnProcessRequestComplete().BindUObject(this, &USFCHttpManager::OnResponseReceivedWithPtr);
	}

	// ��û ����
	Request->ProcessRequest();
}

// data �� ����
FString USFCHttpManager::ExtractDataField(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// Check if the "data" field exists
		TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField(TEXT("data"));

		if (DataObject.IsValid())
		{
			// Convert the "data" object back to a JSON string
			FString DataString;
			TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&DataString);

			if (FJsonSerializer::Serialize(DataObject.ToSharedRef(), JsonWriter))
			{
				return DataString;
			}
		}
	}

	return FString(); // Return an empty string if extraction fails
}

// Request ���� ���ε� �Լ� : ���ڿ�.
void USFCHttpManager::OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// ���� ������ Ȯ��.
	if (bWasSuccessful && Response.IsValid())
	{
		// ����� HttpHandler �ν��Ͻ��� ResultResponseString�� ����.
		FString TempResponseString = Response->GetContentAsString();
		ResultResponseString = ExtractDataField(TempResponseString);

		// ��������Ʈ�� ���ε��� �Լ��� �������� execute()
		if (OnRequestedJsonStringReady.IsBound())
		{
			OnRequestedJsonStringReady.Execute(true);
		}
		OnDynamicRequestingEvent.Broadcast();
	}
	else
	{
		UE_LOG(SFClog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}

// Request ���� ���ε� �Լ� : ��ü ������ : �������Ʈ�� ���÷��� ���� �����Ƿ�, cpp�ܿ��� �߰� �۾� �䱸��.
void USFCHttpManager::OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// ���� ������ Ȯ��
		ResultResponseString = Response->GetContentAsString();

		// JSON �Ľ�
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultResponseString);
		if (FJsonSerializer::Deserialize(Reader, ParsedJsonData))
		{
			// �Ľ� ���� �Լ� ȣ��
			OnParsedJsonObjectPtrReady.Execute(ParsedJsonData);

			// �������Ʈ�� ���̳��� ��Ƽĳ��Ʈ ��������Ʈ
			OnDynamicRequestingEvent.Broadcast();
		}
		else
		{
			UE_LOG(SFClog, Error, TEXT("%s : Failed to parse JSON."), *this->GetName());
		}
	}
	else
	{
		UE_LOG(SFClog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}

// Ŀ���� �Ľ� �Լ� ���� ��������Ʈ.
void USFCHttpManager::ExecuteCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject)
{
	ParsedJsonData = ParseRequestBody(OriginJsonObject);
	if (ParsedJsonData)
	{
		OnParsedJsonObjectPtrReady.Execute(ParsedJsonData);
		OnDynamicRequestingEvent.Broadcast();
	}
}

// �⺻ �Ľ� �Լ�: Ŀ���� �Ľ��� ����Ͽ� ó������ �ٽ� �ۼ��� ��.
TSharedPtr<FJsonObject> USFCHttpManager::ParseRequestBody(TSharedPtr<FJsonObject> RequestBody)
{
	const TSharedPtr<FJsonObject> DataObject = RequestBody;

	if (DataObject.IsValid())
	{
		// JSON ��ü�� ���ڿ��� ���ڵ��Ͽ� JSON �������� ���/
		FString JsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(DataObject.ToSharedRef(), Writer);

		// ����� ���.
		// UE_LOG(JCMlog, Log, TEXT("DataObject JSON: %s"), *JsonString);
	}
	else
	{
		UE_LOG(SFClog, Warning, TEXT("%s : DataObject is invalid"), *this->GetName());
	}
	return DataObject;
}

TMap<FString, FString> USFCHttpManager::ParseJsonStringToMap(const FString& JsonString)
{
	TMap<FString, FString> ParsedMap;
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		for (auto& Elem : JsonObject->Values)
		{
			// FJsonValue�� Ÿ�Կ� ���� ó��
			if (Elem.Value->Type == EJson::String)
			{
				ParsedMap.Add(Elem.Key, Elem.Value->AsString());
			}
			else if (Elem.Value->Type == EJson::Number)
			{
				ParsedMap.Add(Elem.Key, FString::SanitizeFloat(Elem.Value->AsNumber()));
			}
			else if (Elem.Value->Type == EJson::Object)
			{
				// ��ø�� JSON ��ü�� FString���� ��ȯ
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ParsedMap.Add(Elem.Key));
				FJsonSerializer::Serialize(Elem.Value->AsObject().ToSharedRef(), Writer);
			}
			else if (Elem.Value->Type == EJson::Array)
			{
				// �迭�� FString���� ��ȯ
				TArray<TSharedPtr<FJsonValue>> ArrayValues = Elem.Value->AsArray();
				FString ArrayAsString;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ArrayAsString);
				FJsonSerializer::Serialize(ArrayValues, Writer);
				ParsedMap.Add(Elem.Key, ArrayAsString);
			}
			else
			{
				ParsedMap.Add(Elem.Key, TEXT("")); // ��Ÿ ��� �� ���ڿ�
			}
		}
	}
	else
	{
		UE_LOG(SFClog, Warning, TEXT("Failed Json Parsing"));
	}

	return ParsedMap;
}

TArray<FString> USFCHttpManager::ParseStringToStringArray(const FString& ArrayString)
{
	TArray<FString> StringArray;

	// 1. `[` �� `]` ����
	FString CleanString = ArrayString;
	CleanString.RemoveFromStart("[");
	CleanString.RemoveFromEnd("]");

	// 2. ��ǥ�� �������� ���ڿ��� �и�
	CleanString.ParseIntoArray(StringArray, TEXT(","), true);

	// 3. �� ����� �յ� ���� ����
	for (FString& Str : StringArray)
	{
		Str = Str.TrimStartAndEnd();
	}

	return StringArray;
}

TArray<float> USFCHttpManager::ParseStringToFloatArray(const FString& ArrayString)
{
	TArray<float> FloatArray;

	// 1. `[` �� `]` ����
	FString CleanString = ArrayString;
	CleanString.RemoveFromStart("[");
	CleanString.RemoveFromEnd("]");

	// 2. ��ǥ�� �������� ���ڿ��� �и�
	TArray<FString> StringArray;
	CleanString.ParseIntoArray(StringArray, TEXT(","), true);

	// 3. �� ���ڿ� ��Ҹ� float�� ��ȯ�Ͽ� �迭�� �߰�
	for (const FString& Str : StringArray)
	{
		FloatArray.Add(FCString::Atof(*Str));
	}

	return FloatArray;
}

TMap<FString, FString> USFCHttpManager::ParseJsonObjToMap(const TSharedPtr<FJsonObject> OriginJsonObject)
{
	TMap<FString, FString> ParsedMap;
	for (auto& Elem : OriginJsonObject->Values)
	{
		// FJsonValue�� Ÿ�Կ� ���� ó��
		if (Elem.Value->Type == EJson::String)
		{
			ParsedMap.Add(Elem.Key, Elem.Value->AsString());
		}
		else if (Elem.Value->Type == EJson::Number)
		{
			ParsedMap.Add(Elem.Key, FString::SanitizeFloat(Elem.Value->AsNumber()));
		}
		else if (Elem.Value->Type == EJson::Object)
		{
			// ��ø�� JSON ��ü�� FString���� ��ȯ
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ParsedMap.Add(Elem.Key));
			FJsonSerializer::Serialize(Elem.Value->AsObject().ToSharedRef(), Writer);
		}
		else if (Elem.Value->Type == EJson::Array)
		{
			// �迭�� FString���� ��ȯ
			TArray<TSharedPtr<FJsonValue>> ArrayValues = Elem.Value->AsArray();
			FString ArrayAsString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ArrayAsString);
			FJsonSerializer::Serialize(ArrayValues, Writer);
			ParsedMap.Add(Elem.Key, ArrayAsString);
		}
		else
		{
			ParsedMap.Add(Elem.Key, TEXT("")); // ��Ÿ ��� �� ���ڿ�
		}
	}

	return ParsedMap;
}
