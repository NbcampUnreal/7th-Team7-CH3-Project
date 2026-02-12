// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KirboStatComponent.h"
#include "Team7_CH3_Project/UI/DevHUISubSystem.h" // UI 델리게이트 헤더

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

    // 게임 시작 시 UI 서브시스템에 초기 데이터 요청
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
        }
    }
}

void UKirboStatComponent::TakeDamage(float Amount)
{
	CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.0f, BaseStat.MaxHP);
	OnHPChanged.Broadcast(CurrentHP, BaseStat.MaxHP);

    // 데미지 입었을 때 서브시스템을 통해 UI 갱신
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UDevHUISubSystem* UISub = GI->GetSubsystem<UDevHUISubSystem>())
        {
            UISub->BroadcastHPUpdate(CurrentHP, BaseStat.MaxHP);
        }
    }

	if (CurrentHP <= 0.0f)
	{
		//대충 사망 처리
	}
}

void UKirboStatComponent::Heal(float Amount)
{
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, BaseStat.MaxHP);
	OnHPChanged.Broadcast(CurrentHP, BaseStat.MaxHP);

    // 회복했을 때 UI 갱신
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
		return true;
	}
	return false;
}

void UKirboStatComponent::RecoverStamina(float Amount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.0f, BaseStat.MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, BaseStat.MaxStamina);
}
