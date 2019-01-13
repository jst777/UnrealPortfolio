// Fill out your copyright notice in the Description page of Project Settings.

#include "UPGameMode.h"
#include "UPPawn.h"
#include "UPCharacter.h"
#include "UPPlayerController.h"

AUPGameMode::AUPGameMode()
{
	DefaultPawnClass = AUPCharacter::StaticClass();
	PlayerControllerClass = AUPPlayerController::StaticClass();
}

void AUPGameMode::PostLogin(APlayerController* NewPlayer)
{
	UPLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	UPLOG(Warning, TEXT("PostLogin End"));
}