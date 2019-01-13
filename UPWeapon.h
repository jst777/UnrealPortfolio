// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealPortfolio.h"
#include "GameFramework/Actor.h"
#include "UPWeapon.generated.h"

UCLASS()
class UNREALPORTFOLIO_API AUPWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
};
