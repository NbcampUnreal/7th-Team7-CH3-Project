// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboStatComponent.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h"
#include "Team7_CH3_Project/UI/DamageFloatingText.h" // KH 260224 추가 : 플로팅 텍스트


UKirboStatComponent::UKirboStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UKirboStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKirboStatComponent::InitializeStats(const FKirboStatRow& InStatData)
{
	BaseStat = InStatData;

	CurrentHP = BaseStat.MaxHP;
	CurrentStamina = BaseStat.MaxStamina;

	OnHPChanged.Broadcast(CurrentHP, BaseStat.MaxHP);
	OnStaminaChanged.Broadcast(CurrentStamina, BaseStat.MaxStamina);

	UWorld* World = GetWorld();
	if (!World) return;

	if (UGameInstance* GI = World->GetGameInstance())
	{
		UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>();
		if (UISub)
		{
			UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
			UISub->BroadcastStaminaUpdate(CurrentStamina, BaseStat.MaxStamina);
		}
	}
}

void UKirboStatComponent::TakeDamage(float Amount)
{
	CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.0f, BaseStat.MaxHP);
	OnHPChanged.Broadcast(CurrentHP, BaseStat.MaxHP);

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
		{
			UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
		}
	}

	OnDamageReceived.Broadcast(Amount);

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
		{
			UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
		}
	}
}

void UKirboStatComponent::Heal(float Amount)
{
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, BaseStat.MaxHP);
	OnHPChanged.Broadcast(CurrentHP, BaseStat.MaxHP);

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
		{
			UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
		}
	}
}

bool UKirboStatComponent::UseStamina(float Amount)
{
	if (CurrentStamina >= Amount)
	{
		CurrentStamina -= Amount;
		OnStaminaChanged.Broadcast(CurrentStamina, BaseStat.MaxStamina);

		if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
		{
			UISub->BroadcastStaminaUpdate(CurrentStamina, BaseStat.MaxStamina);
		}

		return true;
	}
	return false;
}

void UKirboStatComponent::RecoverStamina(float Amount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.0f, BaseStat.MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, BaseStat.MaxStamina);

	if (UDevHUISubSystem* UISub = GetWorld()->GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
	{
		UISub->BroadcastStaminaUpdate(CurrentStamina, BaseStat.MaxStamina);
	}
}
