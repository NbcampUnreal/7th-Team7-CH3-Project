// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboActionComponent.h"
#include "Character/KirboCharacter.h"
#include "Character/KirboStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UKirboActionComponent::UKirboActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKirboActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKirboActionComponent::Initialize(AKirboCharacter* InCharacter, UKirboStatComponent* InStatComp)
{
	OwnerCharacter = InCharacter;
	StatComp = InStatComp;
}

void UKirboActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 틱마다 스프린트 상태를 확인하여 스태미나 소모 및 회복 처리 (기존 Character의 Tick 로직)
	if (!OwnerCharacter || !StatComp) return;

	float SprintSpeed = StatComp->GetSprintSpeed();
	if (OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed >= SprintSpeed - 1.0f)
	{
		float Cost = StatComp->BaseStat.SprintStaminaCost * DeltaTime;
		if (!StatComp->UseStamina(Cost))
		{
			StopSprint();
		}
	}
	else
	{
		float RecoveryRate = StatComp->GetStaminaRecoveryRate();
		StatComp->RecoverStamina(RecoveryRate * DeltaTime);
	}
}

void UKirboActionComponent::StartSprint()
{
	if (OwnerCharacter && StatComp && StatComp->CurrentStamina > 0)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = StatComp->GetSprintSpeed();
	}
}

void UKirboActionComponent::StopSprint()
{
	if (OwnerCharacter && StatComp)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = StatComp->GetMoveSpeed();
	}
}

void UKirboActionComponent::Dash()
{
	if (!OwnerCharacter || !StatComp) return;

	float DashCost = StatComp->GetDashStaminaCost();
	if (!bCanDash || StatComp->CurrentStamina < DashCost) return;

	bIsInvincible = true;

	FVector Velocity = OwnerCharacter->GetVelocity();
	FVector DashDir = Velocity.IsNearlyZero() ? OwnerCharacter->GetActorForwardVector() : Velocity.GetSafeNormal();
	OwnerCharacter->LaunchCharacter(DashDir * 3000.f, true, true);

	StatComp->UseStamina(DashCost);

	bCanDash = false;
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UKirboActionComponent::ResetDash, 0.5f, false);
}

void UKirboActionComponent::ResetDash()
{
	bCanDash = true;
	bIsInvincible = false;
}

void UKirboActionComponent::StartShot()
{
	if (!OwnerCharacter) return;
	bIsShooting = true;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UKirboActionComponent::StopShot()
{
	if (!OwnerCharacter) return;
	bIsShooting = false;
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UKirboActionComponent::TriggerInvincibility(float Duration)
{
	bIsInvincible = true;
	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &UKirboActionComponent::ResetInvincibility, Duration, false);
}

void UKirboActionComponent::ResetInvincibility()
{
	bIsInvincible = false;
}
