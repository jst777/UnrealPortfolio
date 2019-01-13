// Fill out your copyright notice in the Description page of Project Settings.

#include "UPPawn.h"

// Sets default values
AUPPawn::AUPPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));


	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);
	//-90�� ȸ���ؼ� �� ��ǥ�� �����ش�. (�ƽ����� 3���� ��ǥ��� �𸮾� 3���� ��ǥ�谡 �޶�)
	Mesh->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15, 0, 0));


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}


	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//_C�� �ٿ���� �������Ʈ Ŭ������ �����ü��ִ�.
	//����, ClassFinder�� �����ڿ����� ���Ǿ���Ѵ�. �ƴϸ� ���� ũ���� �߻� 
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
}

// Called when the game starts or when spawned
void AUPPawn::BeginPlay()
{
	Super::BeginPlay();
	
	/*Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Animations/WarriorRun.WarriorRun"));
	if (AnimAsset != nullptr)
	{
		Mesh->PlayAnimation(AnimAsset, true);
	}*/
}

// Called every frame
void AUPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUPPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AUPPawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AUPPawn::LeftRight);
}

void AUPPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPLOG_S(Warning);
}

void AUPPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UPLOG_S(Warning);
}


void AUPPawn::UpDown(float NewAxisValue)
{
	if (NewAxisValue == -1 || NewAxisValue == 1)
	{
		UPLOG(Warning, TEXT("%f"), NewAxisValue);
	}
	AddMovementInput(GetActorForwardVector(), NewAxisValue);
}

void AUPPawn::LeftRight(float NewAxisValue)
{
	if (NewAxisValue == -1 || NewAxisValue == 1)
	{
		UPLOG(Warning, TEXT("%f"), NewAxisValue);
	}

	AddMovementInput(GetActorRightVector(), NewAxisValue);
}