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

    // KH 260224 추가 : 데미지 플로팅 텍스트 소환
    // 플로팅 텍스트 소환 로직
    if (DamageTextClass) // 에디터에서 클래스가 할당되었는지 확인
    {
        FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 100.f);
        ADamageFloatingText* DamageTextActor = GetWorld()->SpawnActor<ADamageFloatingText>(
            DamageTextClass,
            SpawnLocation,
            FRotator::ZeroRotator
        );

        if (DamageTextActor)
        {
            // 성공적으로 소환됨!
            DamageTextActor->SetDamageValue(Amount);
            UE_LOG(LogTemp, Log, TEXT("Floating Text Spawned: %f"), Amount);
        }
        else
        {
            // 소환 실패 (주로 Spawn Collision Handling 때문일 수 있음)
            UE_LOG(LogTemp, Error, TEXT("Failed to Spawn Damage Text Actor!"));
        }
    }
    else
    {
        // 클래스가 할당되지 않음 -> 에디터 확인 필요
        UE_LOG(LogTemp, Warning, TEXT("DamageTextClass is NOT assigned in Editor!"));
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
