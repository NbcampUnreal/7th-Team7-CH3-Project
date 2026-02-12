#include "DevHWeaponInventory.h"
#include "DevHWeaponSlot.h"
#include "DevHUISubSystem.h"

void UDevHWeaponInventory::NativeConstruct()
{
    UE_LOG(LogTemp, Error, TEXT("Inventory UI: NativeConstruct is Running!"));

    Super::NativeConstruct();

    WeaponSlotArray.Empty();
    if (WBP_WeaponSlot) WeaponSlotArray.Add(WBP_WeaponSlot);
    if (WBP_WeaponSlot_1) WeaponSlotArray.Add(WBP_WeaponSlot_1);
    if (WBP_WeaponSlot_2) WeaponSlotArray.Add(WBP_WeaponSlot_2);

    InitializeInventory(); // 초기 데이터 셋팅: 1번만 수행

    if (UDevHUISubSystem* UISub = GetGameInstance()->GetSubsystem<UDevHUISubSystem>())
    {
        // 1. 점수 정보 들어오면 슬롯 해금 여부 판단
        UISub->OnHUDDataChanged.AddDynamic(this, &UDevHWeaponInventory::HandleScoreUpdate);
        // 2. 무기 선택 정보 
        UISub->OnWeaponSelectionChanged.AddDynamic(this, &UDevHWeaponInventory::SelectWeaponSlot);
    }
}

void UDevHWeaponInventory::UpdateWeaponSlot(int32 Index, int32 CurrentScore)
{
    if (WeaponSlotArray.IsValidIndex(Index) && WeaponSlotArray[Index])
    {
        WeaponSlotArray[Index]->UnlockStatus(CurrentScore); // 개별 슬롯 위젯에 점수 넘김 -> 해금 여부 판단
    }
}

void UDevHWeaponInventory::SelectWeaponSlot(int32 SelectedIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("UI Received Weapon Change Index: %d"), SelectedIndex);

    for (int32 i = 0; i < WeaponSlotArray.Num(); i++)
    {
        if (WeaponSlotArray[i])
        {
            // 선택한 인덱스만 UI 강조 활성화
            bool bIsSelected = (i == SelectedIndex);
            WeaponSlotArray[i]->SetSlotActive(bIsSelected);
        }
    }
}

void UDevHWeaponInventory::HandleScoreUpdate(int32 Stage, int32 Wave, int32 Kills, int32 Score)
{
    for (int32 i = 0; i < WeaponSlotArray.Num(); i++)
    {
        if (WeaponSlotArray[i])
        {
            WeaponSlotArray[i]->UnlockStatus(Score);
        }
    }
}

void UDevHWeaponInventory::InitializeInventory()
{
    for (UDevHWeaponSlot* IterSlot : WeaponSlotArray)
    {
        if (IterSlot)
        {
            IterSlot->InitSlotData();
        }
    }
}
