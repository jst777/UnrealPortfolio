// Fill out your copyright notice in the Description page of Project Settings.

#include "UPCharacterStatComponent.h"
#include "UPGameInstance.h"

// Sets default values for this component's properties
UUPCharacterStatComponent::UUPCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//가상함수 InitializeComponent를 호출하게 설정
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UUPCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UUPCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetNewLevel(Level);
}

void UUPCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto UPGameInstance = Cast<UUPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (UPGameInstance != nullptr)
	{
		CurrentStatData = UPGameInstance->GetUPCharacterData(NewLevel);
		if (CurrentStatData != nullptr)
		{
			Level = NewLevel;
			CurrentHP = CurrentStatData->MaxHP;
			SetHP(CurrentHP);
		}
		else
		{
			UPLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
		}
	}
}

void UUPCharacterStatComponent::SetDamage(float NewDamage)
{
	
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0, CurrentStatData->MaxHP));

	//UPLOG(Warning, TEXT("Current HP : %.2f"), CurrentHP);
}

void UUPCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnHPIsZero.Broadcast();
	}
}

float UUPCharacterStatComponent::GetAttack()
{
	return CurrentStatData->Attack;
}

float UUPCharacterStatComponent::GetHPRatio()
{
	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER)  ? 0.f :  (CurrentHP / CurrentStatData->MaxHP);
}