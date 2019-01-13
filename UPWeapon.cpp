// Fill out your copyright notice in the Description page of Project Settings.

#include "UPWeapon.h"

// Sets default values
AUPWeapon::AUPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));


	// ���� ������ ��Ʈ������Ʈ�� ���� ���̷�Ż �޽� ������Ʈ�� ����
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Silly_Weapons/Blade_Sucker/SK_Blade_Sucker.SK_Blade_Sucker"));
	if (SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		//Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
	// ������ü�� �浹���� ���� ����
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

