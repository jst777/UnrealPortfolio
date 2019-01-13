// Fill out your copyright notice in the Description page of Project Settings.

#include "UPPlayerController.h"

void AUPPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UPLOG_S(Warning);

}

void AUPPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);
	UPLOG_S(Warning);
}
 void AUPPlayerController::BeginPlay()
{
	 Super::BeginPlay();

	 // UI배제하고 게임에만 입력 전달되도록 설정.
	 FInputModeGameOnly InputMode;
	 SetInputMode(InputMode);
}