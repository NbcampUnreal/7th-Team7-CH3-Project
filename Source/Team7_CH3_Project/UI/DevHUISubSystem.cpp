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
    // 무기 교체 신호를 보냄
    OnWeaponSelectionChanged.Broadcast(SelectedIndex);
}

void UDevHUISubSystem::BroadcastHPUpdate(float CurrentHP, float MaxHP)
{
    // 체력 변동 신호를 보냄
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}
