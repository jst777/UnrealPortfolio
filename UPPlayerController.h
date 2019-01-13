// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealPortfolio.h"
#include "GameFramework/PlayerController.h"
#include "UPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API AUPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void PostInitializeComponents() override;
	virtual void Possess(APawn* aPawn) override;

	virtual void BeginPlay() override;
};
