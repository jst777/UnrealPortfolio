// Fill out your copyright notice in the Description page of Project Settings.

#include "UPWeapon.h"

// Sets default values
AUPWeapon::AUPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));


	// 무기 액터의 루트컴포넌트는 웨폰 스켈레탈 메시 컴포넌트로 설정
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Silly_Weapons/Blade_Sucker/SK_Blade_Sucker.SK_Blade_Sucker"));
	if (SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		//Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
	// 무기자체는 충돌설정 없게 만듬
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AUPWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
//
//// Called every frame
//void AUPWeapon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

