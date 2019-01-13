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
	//-90도 회전해서 두 좌표계 맞춰준다. (맥스등의 3차원 좌표계와 언리얼 3차원 좌표계가 달라서)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}


	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//_C를 붙여줘야 블루프린트 클래스를 가져올수있다.
	//또한, ClassFinder는 생성자에서만 사용되어야한다. 아니면 엔진 크래시 발생 
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	SetControlMode(EControlMode::DIUPLO);

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;

	//점프 높이를 조절
	GetCharacterMovement()->JumpZVelocity = 800.f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// 충돌 프리셋 변경
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("UPCharacter"));
	AttackRange = 200.f;
	AttackRadius = 50.f;

	CurrentWeapon = nullptr;

	//WidgetBlueprint'/Game/Book/UI/UI_HPBar.UI_HPBar'
	HPBarWidget->SetRelativeLocation(FVector(0, 0, 180.f));
	// 스크린좌표로 설정
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

	// 위젯을 얻어와서 캐릭터스탯 컴포넌트를 바인딩시킨다.
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

			//캐릭터 이동방향으로 자동 회전시켜준다.
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
			SpringArm->bUsePawnControlRotation = false; //컨트롤 회전 사용 금지
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

	// 타겟 길이만큼 보간해준다.
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIUPLO:
		{
			SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
			if (DirectionToMove.SizeSquared() > 0.f)
			{
				// 시선방향(x축)으로 부터 회전행렬을 구축하여 설정한다.
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

	// 인텔리센스가 안먹히네? 매크로함수라 그런가
	// Todo: AddUObject일수도 있나?
	UPAnim->OnMontageEnded.AddDynamic(this, &AUPCharacter::OnAttackMontageEnded);

	UPAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		//UPLOG(Warning, TEXT("OnNextAttackCheck CurrentCombo : %d"), CurrentCombo);
		// 다음콤보 설정을 꺼준다. NextAttackCheck 노티파이가 호출된것이므로...
		CanNextCombo = false;
		//
		if (IsComboInputOn)
		{
			// 콤보상태 초기화
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
			//다음콤보로 넘어갈수 있는 상태면 콤보 인풋 플레그 켜준다.
			IsComboInputOn = true;
		}
	}
	else
	{
		UPCHECK(CurrentCombo == 0);

		//시작 상태로 설정하고
		AttackStartComboState();
		//애님인스턴스에게 공격몽타주 플레이 명령 내린다.
		UPAnim->PlayAttackMontage();
		//현재 코몹에 해당하는 몽타주 섹션으로 점프시킨다.
		UPAnim->JumpToAttackMontageSection(CurrentCombo);

		IsAttacking = true;

	}
	//UPLOG_S(Warning);
}

void AUPCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//몽타쥬 애니메니션이 종료되는 시점에 호출되는 콜백함수로 이때 플레그를 꺼준다.ㄹ
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
	// (단일 충돌판별밖에 안된다.)
	FHitResult HitResult;
	//자신은 충돌판별에서 제외시키기 위해서 this포인터 설정
	FCollisionQueryParams Params(NAME_None, false, this);

	// 시작좌표 및 끝좌표: 액터좌표 및 액터의 전방 200미터
	// 트레이스 채널은 2번이다. (DefaultEngine.ini 참고)
	// 반지름 50센티의 구체를 만들어 검색.
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
	//capsule의 상단벡터(z)가 캐릭터 시선방향과 일치하므로 아래처럼 설정
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, CapsuleRot, DrawColor, false, DebugLifeTime);
#endif
}