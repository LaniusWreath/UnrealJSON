// Fill out your copyright notice in the Description page of Project Settings.

#include "Edu/Public/EduGameInstance.h"

void UEduGameInstance::Init()
{
	TCHAR LogArray[] = TEXT("Hello Unreal");
	UE_LOG(LogTemp, Warning, TEXT("%s"), LogArray);

	FString LogCharString = TEXT("Hello String Unreal");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *LogCharString);
	
	// FString���� TCHAR ��������
	const TCHAR* LogCharPointer = *LogCharString;
	TCHAR* LogCharDataPointer = LogCharString.GetCharArray().GetData();

	// C ��Ÿ�� ���ڿ� ����
	TCHAR LogCharStringSize[100];
	FCString::Strcpy(LogCharStringSize, LogCharString.Len(), *LogCharString);
	
	// ���ڿ� �ڸ���
	if (LogCharString.Contains(TEXT("unreal"), ESearchCase::IgnoreCase))
	{
		// ���� �ε��� �˻�
		int32 Index = LogCharString.Find(TEXT("unreal"), ESearchCase::IgnoreCase);
		// �߰��������� �� ���� �ڸ�
		FString EndString = LogCharString.Mid(Index);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *EndString);
	}

	// ���ڿ� ����
	FString AppendedString = FString::Printf(TEXT("Int: %d, Float : %f"), 32, 3.231412f);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *AppendedString);

}
