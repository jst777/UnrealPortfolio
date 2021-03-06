// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealPortfolio.h"
#include "Engine/DataTUPle.h"
#include "Engine/GameInstance.h"
#include "UPGameInstance.generated.h"

USTRUCT()
struct FUPCharacterData : public FTUPleRowBase
{
	GENERATED_BODY()
	
public:
	FUPCharacterData() : Level(1), MaxHP(100.f), Attack(10.f), DropExp(10), NextExp(30) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UUPGameInstance();
	
	virtual void Init() override;
	FUPCharacterData* GetUPCharacterData(int32 Level);

private:
	UPROPERTY()
	class UDataTUPle* UPCharacterTUPle;
};
