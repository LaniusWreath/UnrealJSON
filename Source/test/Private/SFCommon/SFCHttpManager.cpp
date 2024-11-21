// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCHttpManager.h"
#include "Datas/JCMLog.h"

// ���� request �Լ�
void USFCHttpManager::MakeGetRequest(const FString& Url, const bool GetResultWithFString)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

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

// Request ���� ���ε� �Լ�
void USFCHttpManager::OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// ���� ������ Ȯ��
	if (bWasSuccessful && Response.IsValid())
	{
		// ����� HttpHandler �ν��Ͻ��� ResultResponseString�� ����
		ResultResponseString = Response->GetContentAsString();
		OnRequestedJsonStringReady.Execute(true);
		OnRequestingProcessDone.Broadcast();
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}

// Request ���� ���ε� �Լ�
void USFCHttpManager::OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// ���� ������ Ȯ��
		ResultResponseString = Response->GetContentAsString();
		//UE_LOG(JCMlog, Log, TEXT("Response: %s"), *ResultResponseString);

		TSharedPtr<FJsonObject> JsonData;

		// JSON �Ľ�
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonData) && JsonData.IsValid())
		{
			// �Ľ� ���� �Լ� ȣ��
			ParsedJsonData = JsonData;
			OnParsedJsonObjectPtrReady.Execute(ParsedJsonData);
			OnRequestingProcessDone.Broadcast();
		}
		else
		{
			UE_LOG(JCMlog, Error, TEXT("%s : Failed to parse JSON."), *this->GetName());
		}
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}


void USFCHttpManager::ExecuteCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject)
{
	ParsedJsonData = ParseRequestBody(OriginJsonObject);
	if (ParsedJsonData)
	{
		OnParsedJsonObjectPtrReady.Execute(ParsedJsonData);
		OnRequestingProcessDone.Broadcast();
	}
}

TSharedPtr<FJsonObject> USFCHttpManager::ParseRequestBody(TSharedPtr<FJsonObject> RequestBody)
{
	const TSharedPtr<FJsonObject> DataObject = RequestBody->GetObjectField(TEXT("data"));

	if (DataObject.IsValid())
	{
		// JSON ��ü�� ���ڿ��� ���ڵ��Ͽ� JSON �������� ���
		FString JsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(DataObject.ToSharedRef(), Writer);

		// ����� ���
		//UE_LOG(JCMlog, Log, TEXT("DataObject JSON: %s"), *JsonString);
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : DataObject is invalid"), *this->GetName());
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
		UE_LOG(JCMlog, Warning, TEXT("Failed Json Parsing"));
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
