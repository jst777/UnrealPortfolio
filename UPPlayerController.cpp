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

	 // UI�����ϰ� ���ӿ��� �Է� ���޵ǵ��� ����.
	 FInputModeGameOnly InputMode;
	 SetInputMode(InputMode);
}