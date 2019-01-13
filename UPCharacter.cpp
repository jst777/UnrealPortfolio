// Fill out your copyright notice in the Description page of Project Settings.

#include "UPCharacter.h"
#include "UPAnimInstance.h"
#include "UPWeapon.h"
#include "UPCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "UPCharacterWidget.h"

// Sets default values
AUPCharacter::AUPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UUPCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());
	
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15, 0, 0));

	//Capsule->SetCapsuleHalfHeight(88.f);
	//Capsule->SetCapsuleRadius(34.f);
	//-90�� ȸ���ؼ� �� ��ǥ�� �����ش�. (�ƽ����� 3���� ��ǥ��� �𸮾� 3���� ��ǥ�谡 �޶�)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}


	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//_C�� �ٿ���� �������Ʈ Ŭ������ �����ü��ִ�.
	//����, ClassFinder�� �����ڿ����� ���Ǿ���Ѵ�. �ƴϸ� ���� ũ���� �߻� 
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIUPLO);

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;

	//���� ���̸� ����
	GetCharacterMovement()->JumpZVelocity = 800.f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// �浹 ������ ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("UPCharacter"));
	AttackRange = 200.f;
	AttackRadius = 50.f;

	CurrentWeapon = nullptr;

	//WidgetBlueprint'/Game/Book/UI/UI_HPBar.UI_HPBar'
	HPBarWidget->SetRelativeLocation(FVector(0, 0, 180.f));
	// ��ũ����ǥ�� ����
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}
}

// Called when the game starts or when spawned
void AUPCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ������ ���ͼ� ĳ���ͽ��� ������Ʈ�� ���ε���Ų��.
	auto CharacterWidget = Cast<UUPCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (CharacterWidget != nullptr)
	{
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
}

void AUPCharacter::SetControlMode(EControlMode ControlMode)
{
	CurrentControlMode = ControlMode;
	switch(ControlMode)
	{
	case EControlMode::GTA:
		{
			ArmLengthTo = 450.f;
			//SpringArm->TargetArmLength = 450.f;
			//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			SpringArm->bUsePawnControlRotation = true;

			SpringArm->bInheritYaw = SpringArm->bInheritPitch = SpringArm->bInheritYaw = true;

			SpringArm->bDoCollisionTest = true;
			bUseControllerRotationYaw = false; 

			//ĳ���� �̵��������� �ڵ� ȸ�������ش�.
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0, 720.f, 0);
		}
		break;
	case EControlMode::DIUPLO:
		{
			ArmLengthTo = 800.f;
			ArmRotationTo = FRotator(-45.f, 0, 0);
			//SpringArm->TargetArmLength = 800.f;
			//SpringArm->SetRelativeRotation(FRotator(-45.f, 0, 0));
			SpringArm->bUsePawnControlRotation = false; //��Ʈ�� ȸ�� ��� ����
			SpringArm->bInheritYaw = SpringArm->bInheritPitch = SpringArm->bInheritYaw = false;
			SpringArm->bDoCollisionTest = false;
			bUseControllerRotationYaw = false;// true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0, 720.f, 0);
			
		}
		break;
	}
}

// Called every frame
void AUPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ÿ�� ���̸�ŭ �������ش�.
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIUPLO:
		{
			SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
			if (DirectionToMove.SizeSquared() > 0.f)
			{
				// �ü�����(x��)���� ���� ȸ������� �����Ͽ� �����Ѵ�.
				GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
				AddMovementInput(DirectionToMove);
			}
		}
		break;
	}
}

void AUPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPAnim = Cast<UUPAnimInstance>(GetMesh()->GetAnimInstance());
	UPCHECK(UPAnim != nullptr);

	// ���ڸ������� �ȸ�����? ��ũ���Լ��� �׷���
	// Todo: AddUObject�ϼ��� �ֳ�?
	UPAnim->OnMontageEnded.AddDynamic(this, &AUPCharacter::OnAttackMontageEnded);

	UPAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		//UPLOG(Warning, TEXT("OnNextAttackCheck CurrentCombo : %d"), CurrentCombo);
		// �����޺� ������ ���ش�. NextAttackCheck ��Ƽ���̰� ȣ��Ȱ��̹Ƿ�...
		CanNextCombo = false;
		//
		if (IsComboInputOn)
		{
			// �޺����� �ʱ�ȭ
			AttackStartComboState();
			UPAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	}
	);

	UPAnim->OnAttackHitCheck.AddUObject(this, &AUPCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		UPLOG(Warning, TEXT("OnHPIsZero"));
		UPAnim->SetDeadAnim();
		SetActorEnUPleCollision(false);
	});

	
}

float AUPCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UPLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);

	return FinalDamage;
}

// Called to bind functionality to input
void AUPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AUPCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AUPCharacter::LeftRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AUPCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AUPCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AUPCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AUPCharacter::Attack);
}

bool AUPCharacter::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}

void AUPCharacter::SetWeapon(AUPWeapon* NewWeapon)
{
	UPLOG_S(Warning);
	FName WeaponSocket(TEXT("hand_rsocket"));

	//auto CurWeapon = GetWorld()->SpawnActor<AUPWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	if (NewWeapon != nullptr)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);

		CurrentWeapon = NewWeapon;
	}
}

void AUPCharacter::UpDown(float NewAxisValue)
{
	//AddMovementInput(GetActorForwardVector(), NewAxisValue);
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case EControlMode::DIUPLO:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void AUPCharacter::LeftRight(float NewAxisValue)
{
	//AddMovementInput(GetActorRightVector(), NewAxisValue);
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case EControlMode::DIUPLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	}
}

void AUPCharacter::LookUp(float NewAxisValue)
{
	if (CurrentControlMode == EControlMode::GTA)
	{
		AddControllerPitchInput(NewAxisValue);
	}
}

void AUPCharacter::Turn(float NewAxisValue)
{
	if (CurrentControlMode == EControlMode::GTA)
	{
		AddControllerYawInput(NewAxisValue);
	}
}

void AUPCharacter::ViewChange()
{
	if (CurrentControlMode == EControlMode::GTA)
	{
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIUPLO);
	}
	else
	{
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetControlMode(EControlMode::GTA);
	}

	//UPLOG(Warning, TEXT("ViewChange : %d"), (int)CurrentControlMode);
	//SetControlMode(CurrentControlMode == EControlMode::DIUPLO ? EControlMode::GTA : EControlMode::DIUPLO);
}

void AUPCharacter::Attack()
{
	if (IsAttacking)
	{
		//UPLOG(Warning, TEXT("Attack CanNextCombo : %d"), CanNextCombo? 1 : 0);


		if (CanNextCombo)
		{
			//�����޺��� �Ѿ�� �ִ� ���¸� �޺� ��ǲ �÷��� ���ش�.
			IsComboInputOn = true;
		}
	}
	else
	{
		UPCHECK(CurrentCombo == 0);

		//���� ���·� �����ϰ�
		AttackStartComboState();
		//�ִ��ν��Ͻ����� ���ݸ�Ÿ�� �÷��� ��� ������.
		UPAnim->PlayAttackMontage();
		//���� �ڸ��� �ش��ϴ� ��Ÿ�� �������� ������Ų��.
		UPAnim->JumpToAttackMontageSection(CurrentCombo);

		IsAttacking = true;

	}
	//UPLOG_S(Warning);
}

void AUPCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//��Ÿ�� �ִϸ޴ϼ��� ����Ǵ� ������ ȣ��Ǵ� �ݹ��Լ��� �̶� �÷��׸� ���ش�.��
	UPCHECK(IsAttacking);
	UPCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void AUPCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	UPCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AUPCharacter::AttackEndComboState()
{
	CanNextCombo = false;
	IsComboInputOn = false;
	CurrentCombo = 0;

	UPLOG_S(Warning);
}

void AUPCharacter::AttackCheck()
{
	// (���� �浹�Ǻ��ۿ� �ȵȴ�.)
	FHitResult HitResult;
	//�ڽ��� �浹�Ǻ����� ���ܽ�Ű�� ���ؼ� this������ ����
	FCollisionQueryParams Params(NAME_None, false, this);

	// ������ǥ �� ����ǥ: ������ǥ �� ������ ���� 200����
	// Ʈ���̽� ä���� 2���̴�. (DefaultEngine.ini ����)
	// ������ 50��Ƽ�� ��ü�� ����� �˻�.
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 200.f,
		FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.f), Params);
	
	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			UPLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
#if ENUPLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	//capsule�� ��ܺ���(z)�� ĳ���� �ü������ ��ġ�ϹǷ� �Ʒ�ó�� ����
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, CapsuleRot, DrawColor, false, DebugLifeTime);
#endif
}