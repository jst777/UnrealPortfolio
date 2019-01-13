// Fill out your copyright notice in the Description page of Project Settings.

#include "UPItemBox.h"
#include "UPCharacter.h"
#include "UPWeapon.h"

// Sets default values
AUPItemBox::AUPItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/BreakUPles/StaticMesh/Box/SM_Env_BreakUPles_Box1.SM_Env_BreakUPles_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->SetAutoActivate(false);
	}

	Box->SetRelativeLocation(FVector(0, -3.5f, -30.f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AUPWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AUPItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUPItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AUPItemBox::OnCharacterOverlap);
}

void AUPItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//UPLOG_S(Warning);

	auto UPCharcter = Cast<AUPCharacter>(OtherActor);

	//UPLOG(Warning, TEXT("character = %d WeaponItemCalss = %d"), UPCharcter == nullptr ? 0 : 1, WeaponItemClass == nullptr ? 0 : 1);

	if (UPCharcter != nullptr && WeaponItemClass != nullptr)
	{
		if (UPCharcter->CanSetWeapon())
		{
			auto NewWeapon = GetWorld()->SpawnActor<AUPWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			UPCharcter->SetWeapon(NewWeapon);
			//Effect 발동 및 콜백등록
			Effect->Activate(true);
			Box->SetHiddenInGame(true, true);
			SetActorEnUPleCollision(false);
			Effect->OnSystemFinished.AddDynamic(this, &AUPItemBox::OnEffectFinished);
		}
		else
		{
			UPLOG(Warning, TEXT("%s can't equip weapon currently"), *UPCharcter->GetName());
		}
	}
}

void AUPItemBox::OnEffectFinished(class UParticleSystemComponent* PSystem)
{
	//이펙트가 종료되면 액터를 제거한다.
	Destroy();
}