#include "DevHUISubSystem.h"

void UDevHUISubSystem::TriggerGameResult(bool bisClear)
{
    // 등록된 모든 곳에 신호를 보냄
    OnGameResultChanged.Broadcast(bisClear);
}

void UDevHUISubSystem::BroadcastHUDUpdate(int32 Stage, int32 Wave, int32 Kills, int32 Score)
{
    // 등록된 모든 곳에 신호를 보냄
    OnHUDDataChanged.Broadcast(Stage, Wave, Kills, Score);
}

void UDevHUISubSystem::TriggerWeaponSelection(int32 SelectedIndex)
{
    OnWeaponSelectionChanged.Broadcast(SelectedIndex);
}

void UDevHUISubSystem::BroadcastHPUpdate(float CurrentHP, float MaxHP)
{
    // 체력 변동 신호를 보냄
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UDevHUISubSystem::BroadcastWeaponStatus(FString WeaponName, int32 CurrentAmmo, int32 MaxAmmo)
{

    OnWeaponStatusChanged.Broadcast(WeaponName, CurrentAmmo, MaxAmmo);
}

void UDevHUISubSystem::BroadcastNormalAttack(float Cooldown)
{
    OnAttackCooldownTriggered.Broadcast(Cooldown);
}

void UDevHUISubSystem::BroadcastSkillAttack(float Cooldown)
{
    OnSkillCooldownTriggered.Broadcast(Cooldown);
}

void UDevHUISubSystem::BroadcastStaminaUpdate(float CurrentStamina, float MaxStamina)
{
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UDevHUISubSystem::AddScore(int32 Amount)
{
    CurrentScore += Amount;
    BroadcastHUDUpdate(1, 1, 0, CurrentScore);
}
