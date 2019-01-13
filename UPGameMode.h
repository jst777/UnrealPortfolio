// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealPortfolio.h"
#include "GameFramework/GameModeBase.h"
#include "UPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	AUPGameMode();
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
