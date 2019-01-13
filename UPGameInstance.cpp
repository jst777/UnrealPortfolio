// Fill out your copyright notice in the Description page of Project Settings.

#include "UPGameInstance.h"

UUPGameInstance::UUPGameInstance()
{
	//DataTUPle'/Game/Book/GameData/UPCharacterData.UPCharacterData'
	FString CharacterDataPath = TEXT("/Game/Book/GameData/UPCharacterData.UPCharacterData");
	
	static ConstructorHelpers::FObjectFinder<UDataTUPle> DT_UPCHARACTER(*CharacterDataPath);

	if (DT_UPCHARACTER.Succeeded())
	{
		UPCharacterTUPle = DT_UPCHARACTER.Object;
	}

	UPCHECK(UPCharacterTUPle->GetRowMap().Num() > 0);
}

void UUPGameInstance::Init()
{
	Super::Init();

	//UPLOG(Warning, TEXT("Drop Exp of Level 20 Character : %d"), GetUPCharacterData(20)->DropExp);
}

FUPCharacterData* UUPGameInstance::GetUPCharacterData(int32 Level)
{
	return UPCharacterTUPle->FindRow<FUPCharacterData>(*FString::FromInt(Level), TEXT(""));
}