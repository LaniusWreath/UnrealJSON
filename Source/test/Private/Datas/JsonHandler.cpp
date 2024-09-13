// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JsonHandler.h"

TSharedPtr<FJsonObject> UJsonHandler::GetJsonObjectData(const FString& FilePath)
{
	FString JsonData;
    TSharedPtr<FJsonObject> ParsedData;
    if (FFileHelper::LoadFileToString(JsonData, *FilePath))
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonData);

        // Json ÀÐ±â
        if (FJsonSerializer::Deserialize(Reader, ParsedData))
        {
            UE_LOG(LogTemp, Log, TEXT("JSONHandler : JSON Data Parsed Successfully"));
            return ParsedData;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("JSONHander : Failed to Parse JSON Data"));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("JSONHandler : Failed to Read JSON Data"));
        return nullptr;
    }
}