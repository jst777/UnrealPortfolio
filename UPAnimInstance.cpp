// Fill out your copyright notice in the Description page of Project Settings.

#include "UPAnimInstance.h"

UUPAnimInstance::UUPAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	//AnimMontage'/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

	IsDead = false;
}

void UUPAnimInstance::PlayAttackMontage()
{
	// ��������Ʈ�� ���� ���� ���� �����ǹǷ� �� ����üũ�� �ʿ� ����.
	//if (!Montage_IsPlaying(AttackMontage))
	{
		Montage_Play(AttackMontage);
	}
}

void UUPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();

	if (!::IsValid(Pawn))
		return;

	if (!IsDead)
	{	
		//���� �ӵ��� �ӷ°��� �����´�.
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character != nullptr)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

// �ִϸ��̼� ��Ƽ����. 
void UUPAnimInstance::AnimNotify_AttackHitCheck()
{
	//UPLOG_S(Warning);

	OnAttackHitCheck.Broadcast();
}

void UUPAnimInstance::AnimNotify_NextAttackCheck()
{
	//UPLOG_S(Warning);
	OnNextAttackCheck.Broadcast();
}


void UUPAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	UPCHECK(Montage_IsPlaying(AttackMontage));

	Montage_JumpToSection(GetAttackMontageSectionName(NewSection));
}

FName UUPAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	UPCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}