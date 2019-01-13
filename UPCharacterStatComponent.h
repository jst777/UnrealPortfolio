// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealPortfolio.h"
#include "Components/ActorComponent.h"
#include "UPCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnUPleComponent) )
class UNREALPORTFOLIO_API UUPCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUPCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	void SetHP(float NewHP);
	float GetAttack();
	float GetHPRatio();

	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangedDelegate OnHPChanged;

private:
	struct FUPCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta= (AllowPrivateAccess = true))
	int32 Level;

	//Transient�� ����ȭ���� ���ܽ�Ŵ (
	UPROPERTY(Transient, VisibleInstanceOnly,  Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};
